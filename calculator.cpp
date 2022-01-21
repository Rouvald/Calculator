#include "calculator.h"

Calculator::Calculator(QWidget* parent) : QWidget(parent)
{
    Display = new QLineEdit();
    Display->setReadOnly(true);
    Display->setAlignment(Qt::AlignRight);
    Display->setMaxLength(20);
    Display->setStyleSheet("font-size: 32px");

    DisplayTotal = new QLineEdit();
    DisplayTotal->setReadOnly(true);
    DisplayTotal->setAlignment(Qt::AlignRight);
    DisplayTotal->setMaxLength(20);
    DisplayTotal->setStyleSheet("font-size: 32px");

    QFont Font = Display->font();
    Font.setPointSize(Font.pointSize() + 8);
    Display->setFont(Font);
    DisplayTotal->setFont(Font);

    for (int32_t index = 0; index < NumDigitButtons; ++index)
    {
        DigitButtons[index] = CreateButton(QString::number(index), SLOT(DigitClicked()));
    }
    Button* PlusButton = CreateButton(tr("+"), SLOT(AdditiveOperationClicked()));
    Button* MinusButton = CreateButton(tr("-"), SLOT(AdditiveOperationClicked()));
    Button* MultyButton = CreateButton(tr("\303\227"), SLOT(MultiOperationClicked()));
    Button* DivisionButton = CreateButton(tr("\303\267"), SLOT(MultiOperationClicked()));

    Button* SquareRootButton = CreateButton(tr("Sqrt"), SLOT(UnaryOperationClicked()));
    Button* PowerButton = CreateButton(tr("x\302\262"), SLOT(UnaryOperationClicked()));
    Button* ReciprocalButton = CreateButton(tr("1/x"), SLOT(UnaryOperationClicked()));

    Button* EqualButton = CreateButton(tr("="), SLOT(EqualClicked()));

    Button* PointButton = CreateButton(tr("."), SLOT(PointClicked()));
    Button* ChangeSignButton = CreateButton(tr("\302\261"), SLOT(ChangeSignClicked()));

    Button* BackspaceButton = CreateButton(tr("Backspace"), SLOT(BackspaceClicked()));
    Button* ClearButton = CreateButton(tr("Clear"), SLOT(ClearClicked()));
    Button* ClearAllButton = CreateButton(tr("Clear All"), SLOT(ClearAllClicked()));

    // Make New GridLayout and fill
    QGridLayout* MainLayout = new QGridLayout;
    MainLayout->setSizeConstraint(QLayout::SetFixedSize);

    MainLayout->addWidget(Display, 0, 0, 1, 5);
    MainLayout->addWidget(DisplayTotal, 1, 0, 1, 5);

    MainLayout->addWidget(ClearAllButton, 2, 0, 1, 2);
    MainLayout->addWidget(ClearButton, 2, 2);
    MainLayout->addWidget(BackspaceButton, 2, 3, 1, 2);

    MainLayout->addWidget(DivisionButton, 3, 3);
    MainLayout->addWidget(MultyButton, 4, 3);
    MainLayout->addWidget(MinusButton, 5, 3);
    MainLayout->addWidget(PlusButton, 6, 3);

    MainLayout->addWidget(SquareRootButton, 3, 4);
    MainLayout->addWidget(PowerButton, 4, 4);
    MainLayout->addWidget(ReciprocalButton, 5, 4);
    MainLayout->addWidget(EqualButton, 6, 4);

    for (int32_t index = 1; index < NumDigitButtons; ++index)
    {
        int32_t Row{5 - ((index - 1) / 3)};
        int32_t Col{((index - 1) % 3)};
        MainLayout->addWidget(DigitButtons[index], Row, Col);
    }

    MainLayout->addWidget(PointButton, 6, 0);
    MainLayout->addWidget(DigitButtons[0], 6, 1);
    MainLayout->addWidget(ChangeSignButton, 6, 2);

    setLayout(MainLayout);
}

Calculator::~Calculator() {}

void Calculator::DigitClicked()
{
    const Button* ClickedButton = GetButton(sender());
    if (!ClickedButton) return;

    const int32_t DigitValue = ClickedButton->text().toInt();

    // Can't type more then 1 "0"
    if (Display->text() == "0" && DigitValue == 0) return;

    Display->setText(Display->text() + QString::number(DigitValue));
    AllValues[CurrentValueIndex].append(ClickedButton->text());

    CalculateAllOperation();
    if (!AllOperations.isEmpty())
    {
        AbortOperation();
        return;
    }

    //    if (WaitingForValue)
    //    {
    //        WaitingForValue = false;
    //    }
}

void Calculator::CalculateAllOperation()
{
    if (!AllOperations.isEmpty() && AllValues.size() == (AllOperations.size() + 1))
    {
        for (int32_t Index = 0; Index <= AllOperations.size(); ++Index)
        {
            if (AllOperations[Index] == tr("\303\227") || AllOperations[Index] == tr("\303\267"))
            {
                double LeftValue{AllValues[Index].toDouble()};
                if (!Calculate(LeftValue, AllValues[Index + 1].toDouble(), AllOperations[Index]))
                {
                    AbortOperation();
                    return;
                }
                AllValues[Index] = QString::number(LeftValue);
                AllValues.removeAt(Index + 1);
                // TODO: check removed operation
                AllOperations.remove(Index, 1);
            }
        }
        if (!AllOperations.contains(tr("\303\227")) && !AllOperations.contains(tr("\303\267")) &&
            (AllOperations.contains(tr("+")) || AllOperations.contains(tr("-"))))
        {
            for (int32_t Index = 0; Index <= AllOperations.size(); ++Index)
            {
                double LeftValue{AllValues[Index].toDouble()};
                if (!Calculate(LeftValue, AllValues[Index + 1].toDouble(), AllOperations[Index]))
                {
                    AbortOperation();
                    return;
                }
                AllValues[Index] = QString::number(LeftValue);
                AllValues.removeAt(Index + 1);
                AllOperations.remove(Index, 1);
            }
        }
        if (AllValues.size() == 1)
        {
            DisplayTotal->setText(AllValues[0]);
        }
    }
}

void Calculator::UnaryOperationClicked()
{
    const Button* ClickedButton = GetButton(sender());
    if (!ClickedButton) return;

    const QString ClickedOperation = ClickedButton->text();

    if (AllValues.isEmpty()) return;

    double Value{AllValues[CurrentValueIndex].toDouble()};
    double Result = 0.0;

    if (ClickedOperation == tr("Sqrt"))
    {
        if (Value < 0.0)
        {
            AbortOperation();
            return;
        }
        Result = std::sqrt(Value);
        Display->setText(ClickedOperation + tr("(") + QString::number(Value) + tr(")"));
    }
    else if (ClickedOperation == tr("x\302\262"))
    {
        Result = pow(Value, 2.0);
        Display->setText(QString::number(Value) + tr("\302\262"));
    }
    else if (ClickedOperation == tr("1/x"))
    {
        if (Value == 0.0)
        {
            AbortOperation();
            return;
        }
        Result = 1 / Value;
        Display->setText(tr("1/") + QString::number(Value));
    }
    DisplayTotal->setText(QString::number(Result));
    WaitingForValue = true;
}

void Calculator::AdditiveOperationClicked()
{
    const Button* ClickedButton = GetButton(sender());
    if (!ClickedButton) return;

    const QString ClickedOperation = ClickedButton->text();
    /*
        double Value = Display->text().toDouble();
        if (!PendingMultiOperation.isEmpty())
        {
            Value = DisplayTotal->text().toDouble();
            RightValue.clear();
        }
        if (!PendingAdditiveOperation.isEmpty())
        {
            Value = DisplayTotal->text().toDouble();
            RightValue.clear();
        }
        Display->setText(Display->text() + ClickedOperation);
    */
    /*
    if (!PendingMultiOperation.isEmpty())
    {
        if (!Calculate(Value, PendingMultiOperation))
        {
            AbortOperation();
            return;
        }
        DisplayTotal->setText(QString::number(FactorSoFar));
        Value = FactorSoFar;
        FactorSoFar = 0.0;
        PendingMultiOperation.clear();
    }
*/

    /*
    SumSoFar = Value;

    PendingAdditiveOperation = ClickedOperation;
    LastOperation = ClickedOperation;
    WaitingForValue = true;
    */
}

void Calculator::MultiOperationClicked()
{
    const Button* ClickedButton = GetButton(sender());
    if (!ClickedButton) return;
    /*
        const QString ClickedOperation = ClickedButton->text();
        double Value = Display->text().toDouble();
        if (!PendingMultiOperation.isEmpty())
        {
            Value = DisplayTotal->text().toDouble();
            RightValue.clear();
        }
        Display->setText(Display->text() + ClickedOperation);

        FactorSoFar = Value;

        PendingMultiOperation = ClickedOperation;
        LastOperation = ClickedOperation;
        WaitingForValue = true;
    */
}

void Calculator::EqualClicked()
{
    double Value = DisplayTotal->text().toDouble();
    /*
        if (!PendingMultiOperation.isEmpty())
        {
            if (!Calculate(Value, PendingMultiOperation))
            {
                AbortOperation();
                return;
            }
            Value = FactorSoFar;
            FactorSoFar = 0.0;
            PendingMultiOperation.clear();
        }
        if (!PendingAdditiveOperation.isEmpty())
        {
            if (!Calculate(Value, PendingAdditiveOperation))
            {
                AbortOperation();
                return;
            }
            PendingAdditiveOperation.clear();
        }
        else
        {
            SumSoFar = Value;
        }
        DisplayTotal->setText(QString::number(SumSoFar));
        SumSoFar = 0.0;
        WaitingForValue = true;
    */
}

void Calculator::PointClicked()
{
    if (WaitingForValue)
    {
        DisplayTotal->setText(tr("0"));
    }
    if (!DisplayTotal->text().contains(tr(".")))
    {
        DisplayTotal->setText(DisplayTotal->text() + tr("."));
    }
    WaitingForValue = false;
}

void Calculator::ChangeSignClicked()
{
    QString Text = DisplayTotal->text();
    double Value = DisplayTotal->text().toDouble();

    if (Value > 0.0)
    {
        Text.prepend(tr("-"));
    }
    else if (Value < 0.0)
    {
        Text.remove(0, 1);
    }
    DisplayTotal->setText(Text);
}

void Calculator::BackspaceClicked()
{
    // if (WaitingForValue) return;

    QString Text = Display->text();
    Text.chop(1);
    if (Text.isEmpty())
    {
        Text = tr("0");
        WaitingForValue = true;
    }
    Display->setText(Text);
}

void Calculator::ClearClicked()
{
    // if (WaitingForValue) return;

    DisplayTotal->setText(tr("0"));
    WaitingForValue = true;
}

void Calculator::ClearAllClicked()
{
    DisplayTotal->setText(QString());
    WaitingForValue = true;
}

Button* Calculator::CreateButton(const QString& ButtonName, const char* Member)
{
    Button* NewButton = new Button(ButtonName);
    connect(NewButton, SIGNAL(clicked()), this, Member);
    return NewButton;
}

bool Calculator::Calculate(double& LeftValue, double RightValue, const QString& PendingOperation)
{
    if (PendingOperation == tr("+"))
    {
        LeftValue += RightValue;
    }
    else if (PendingOperation == tr("-"))
    {
        LeftValue -= RightValue;
    }
    else if (PendingOperation == tr("\303\227"))
    {
        LeftValue *= RightValue;
    }
    else if (PendingOperation == tr("\303\267"))
    {
        if (RightValue == 0.0) return false;
        LeftValue /= RightValue;
    }
    return true;
}

void Calculator::AbortOperation()
{
    ClearAllClicked();
    DisplayTotal->setText(tr("error"));
}

Button* Calculator::GetButton(QObject* Sender) const
{
    return qobject_cast<Button*>(Sender);
}
