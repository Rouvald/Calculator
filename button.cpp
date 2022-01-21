#include "button.h"

Button::Button(const QString& ButtonName, QWidget* parent) : QToolButton(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setText(ButtonName);
    setStyleSheet("font-size: 24px");
}

QSize Button::sizeHint() const
{
    QSize Size = QToolButton::sizeHint();
    Size.rheight() = 80;
    Size.rwidth() = qMax(Size.width(), Size.height());
    return Size;
}
