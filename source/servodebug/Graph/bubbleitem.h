#ifndef BUBBLEITEM_H
#define BUBBLEITEM_H

#include <QGraphicsItem>
#include <QFont>

class BubbleItem : public QGraphicsItem
{
private:
    QFont m_font;
    QString m_str;
    QRectF m_strRect;
    void calcStrRect();

public:
    BubbleItem();
    BubbleItem(const QFont &font, const QString &str);

    void setFont(const QFont &font);
    void setStr(const QString &str);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget);
};

#endif // BUBBLEITEM_H
