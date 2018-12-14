#ifndef PROGRESSBARDELEGATE_H
#define PROGRESSBARDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QApplication>

class ProgressBarDelegate : public QStyledItemDelegate
{
public:
    ProgressBarDelegate(QObject* parent = nullptr);

    /* Метод отрисовки прогресс бара:
     * painter - Объект класса QPainter, которым рисуем индикатор
     * QStyleOptionViewItem - Набор опций для отображения ячейки QStyleOptionViewItem
     * index - можно использовать как индекс модели QModelIndex, с помощью которого
     * можно получить всю необходимую информацию о ячейке таблицы, которую предстоит нарисовать.
     * Или передаём просто значение для отображения.
     */
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // PROGRESSBARDELEGATE_H
