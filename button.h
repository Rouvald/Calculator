#ifndef BUTTON_H
#define BUTTON_H

#include <QToolButton>
#include <QtCore>

class Button : public QToolButton
{
    Q_OBJECT
public:
    explicit Button(const QString& ButtonName, QWidget* parent = nullptr);

    virtual QSize sizeHint() const override;
};

#endif // BUTTON_H
