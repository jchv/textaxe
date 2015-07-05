#include <QTextBlock>
#include <QPainter>

#include "textedit.h"

/* Gutter widget */
Gutter::Gutter(TextEdit *textEdit)
    : QWidget(textEdit), textEdit(textEdit)
{
}

QSize Gutter::sizeHint() const
{
    return QSize(textEdit->gutterWidth(), 0);
}

void Gutter::paintEvent(QPaintEvent *event)
{
    textEdit->gutterPaintEvent(event);
}

/* TextEdit widget */
TextEdit::TextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
{
    gutter = new Gutter(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateGutterWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateGutter(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateGutterWidth(0);
    highlightCurrentLine();
}

void TextEdit::gutterPaintEvent(QPaintEvent *event)
{
    QPainter painter(gutter);
    painter.fillRect(event->rect(), QColor(0xee, 0xee, 0xee));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    int digiW = fontMetrics().width(QLatin1Char('9'));

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(0x88, 0x88, 0x88));
            painter.drawText(0, top, gutter->width() - digiW / 2, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int TextEdit::gutterWidth()
{
    int blocks = blockCount();
    int digits = 1;
    while (blocks /= 10)
        digits++;
    return fontMetrics().width(QLatin1Char('9')) * (qMax(digits, 3) + 1);
}

void TextEdit::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    gutter->setGeometry(QRect(cr.left(), cr.top(), gutterWidth(), cr.height()));
}

void TextEdit::updateGutterWidth(int newBlockCount)
{
    setViewportMargins(gutterWidth(), 0, 0, 0);
}

void TextEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(190);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void TextEdit::updateGutter(const QRect &rect, int dy)
{
    if (dy)
        gutter->scroll(0, dy);
    else
        gutter->update(0, rect.y(), gutter->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateGutterWidth(0);
}
