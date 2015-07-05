#pragma once

#include <QPlainTextEdit>

class Gutter;
class TextEdit;

class Gutter : public QWidget
{
    Q_OBJECT

public:
    explicit Gutter(TextEdit *textEdit);
    QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    TextEdit *textEdit;
};

class TextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit TextEdit(QWidget *parent = 0);

    void gutterPaintEvent(QPaintEvent *event);
    int gutterWidth();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void updateGutterWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateGutter(const QRect &rect, int dy);

private:
    QWidget *gutter;
};
