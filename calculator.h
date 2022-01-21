#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QtCore>
#include <QtWidgets>
#include "button.h"
#include <QDebug>

class Calculator : public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget* parent = nullptr);
    ~Calculator();

private slots:
    void DigitClicked();
    void UnaryOperationClicked();
    void AdditiveOperationClicked();
    void MultiOperationClicked();
    void EqualClicked();
    void PointClicked();
    void ChangeSignClicked();
    void BackspaceClicked();
    void ClearClicked();
    void ClearAllClicked();

private:
    double SumSoFar{0.0};
    double FactorSoFar{0.0};
    QString PendingAdditiveOperation;
    QString PendingMultiOperation;
    bool WaitingForValue{true};

    QLineEdit* Display;
    QLineEdit* DisplayTotal;
    enum : int32_t
    {
        NumDigitButtons = 10
    };
    Button* DigitButtons[NumDigitButtons];

    Button* CreateButton(const QString& ButtonName, const char* Member);
    void AbortOperation();
    bool Calculate(double RightValue, const QString& PendingOperation);

    Button* GetButton(QObject* Sender) const;
};

#endif // CALCULATOR_H
