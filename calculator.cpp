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
    Button* PlusButton = CreateButton(tr("+"), SLOT(BinaryOperationClicked()));
    Button* MinusButton = CreateButton(tr("-"), SLOT(BinaryOperationClicked()));
    Button* MultyButton = CreateButton(tr("\303\227"), SLOT(BinaryOperationClicked()));
    Button* DivisionButton = CreateButton(tr("\303\267"), SLOT(BinaryOperationClicked()));

    Button* SquareRootButton = CreateButton(tr("\u221A"), SLOT(UnaryOperationClicked()));
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

    Display->text() == "0" ? Display->setText(QString::number(DigitValue))
                           : Display->setText(Display->text() + QString::number(DigitValue));

    if (AllValues.size() == CurrentValueIndex)
    {
        AllValues.append(QString(ClickedButton->text()));
    }
    else if (AllValues.size() == CurrentValueIndex + 1)
    {
        AllValues[CurrentValueIndex].append(ClickedButton->text());
    }
    else if (CurrentValueIndex < 0 || CurrentValueIndex > AllValues.size())
    {
        AbortOperation();
        return;
    }
    CalculateAllOperation();

    if (WaitingForValue)
    {
        WaitingForValue = false;
    }
}

void Calculator::CalculateAllOperation()
{
    QList<QString> AllTotalValues = AllValues;
    QString AllTotalOperations = AllOperations;

    if (!AllTotalOperations.isEmpty() && !AllTotalOperations.isEmpty() && AllTotalValues.size() == (AllTotalOperations.size() + 1))
    {
        if (AllTotalOperations.contains(tr("\303\227")) || AllTotalOperations.contains(tr("\303\267")))
        {
            for (int32_t Index = 0; Index < AllTotalOperations.size(); ++Index)
            {
                if (AllTotalOperations[Index] == tr("\303\227") || AllTotalOperations[Index] == tr("\303\267"))
                {
                    double LeftValue{AllTotalValues[Index].toDouble()};
                    if (!Calculate(LeftValue, AllTotalValues[Index + 1].toDouble(), AllTotalOperations[Index]))
                    {
                        AbortOperation();
                        return;
                    }
                    AllTotalValues[Index] = QString::number(LeftValue);
                    AllTotalValues.removeAt(Index + 1);
                    // TODO: check removed operation
                    AllTotalOperations.remove(Index, 1);
                    --Index;
                }
            }
        }
        if (!AllTotalOperations.contains(tr("\303\227")) && !AllTotalOperations.contains(tr("\303\267")) &&
            (AllTotalOperations.contains(tr("+")) || AllTotalOperations.contains(tr("-"))))
        {
            for (int32_t Index = 0; Index < AllTotalOperations.size(); ++Index)
            {
                double LeftValue{AllTotalValues[Index].toDouble()};
                if (!Calculate(LeftValue, AllTotalValues[Index + 1].toDouble(), AllTotalOperations[Index]))
                {
                    AbortOperation();
                    return;
                }
                AllTotalValues[Index] = QString::number(LeftValue);
                AllTotalValues.removeAt(Index + 1);
                AllTotalOperations.remove(Index, 1);
                --Index;
            }
        }
        if (AllTotalValues.size() == 1 && AllTotalOperations.isEmpty())
        {
            DisplayTotal->setText(AllTotalValues[0]);
        }
    }
}

void Calculator::UnaryOperationClicked()
{
    const Button* ClickedButton = GetButton(sender());
    if (!ClickedButton) return;

    const QString ClickedOperation = ClickedButton->text();

    if (AllValues.isEmpty() || WaitingForValue) return;

    double Value{AllValues[CurrentValueIndex].toDouble()};
    double Result = 0.0;

    if (ClickedOperation == tr("\u221A"))
    {
        if (Value < 0.0)
        {
            AbortOperation();
            return;
        }
        Result = std::sqrt(Value);
        int32_t RemovePosition = Display->text().size() - AllValues[CurrentValueIndex].size();
        Display->setText(Display->text().remove(RemovePosition, AllValues[CurrentValueIndex].size()));
        Display->setText(Display->text() + ClickedOperation + QString::number(Value));
        AllValues[CurrentValueIndex] = QString::number(Result);
    }
    else if (ClickedOperation == tr("x\302\262"))
    {
        Result = pow(Value, 2.0);
        int32_t RemovePosition = Display->text().size() - AllValues[CurrentValueIndex].size();
        Display->setText(Display->text().remove(RemovePosition, AllValues[CurrentValueIndex].size()));
        Display->setText(Display->text() + QString::number(Value) + tr("\302\262"));
        AllValues[CurrentValueIndex] = QString::number(Result);
    }
    else if (ClickedOperation == tr("1/x"))
    {
        if (Value == 0.0)
        {
            AbortOperation();
            return;
        }
        Result = 1 / Value;
        int32_t RemovePosition = Display->text().size() - AllValues[CurrentValueIndex].size();
        Display->setText(Display->text().remove(RemovePosition, AllValues[CurrentValueIndex].size()));
        Display->setText(Display->text() + tr("1/") + QString::number(Value));
        AllValues[CurrentValueIndex] = QString::number(Result);
    }
    CalculateAllOperation();
    if (AllValues.size() == 1 && AllOperations.isEmpty())
    {
        DisplayTotal->setText(AllValues[0]);
    }
    WaitingForValue = true;
}

void Calculator::BinaryOperationClicked()
{
    if (WaitingForValue) return;

    const Button* ClickedButton = GetButton(sender());
    if (!ClickedButton) return;

    const QString ClickedOperation = ClickedButton->text();

    Display->setText(Display->text() + ClickedOperation);

    ++CurrentValueIndex;
    AllOperations.append(ClickedOperation);
    WaitingForValue = true;
}

/*
void Calculator::MultiOperationClicked()
{
    const Button* ClickedButton = GetButton(sender());
    if (!ClickedButton) return;

    const QString ClickedOperation = ClickedButton->text();

    Display->setText(Display->text() + ClickedOperation);

    ++CurrentValueIndex;
    AllOperations.append(ClickedOperation);
    WaitingForValue = true;
}
*/

void Calculator::EqualClicked()
{
    // double Value = DisplayTotal->text().toDouble();
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
    if (WaitingForValue && AllValues.isEmpty())
    {
        Display->setText(tr("0"));
        AllValues.append(QString{"0"});
    }
    if (!AllValues[CurrentValueIndex].contains(tr(".")))
    {
        Display->setText(Display->text() + tr("."));
        AllValues[CurrentValueIndex].append(tr("."));
    }
    WaitingForValue = false;
}

void Calculator::ChangeSignClicked()
{
    if (WaitingForValue) return;

    if (AllValues[CurrentValueIndex].toDouble() >= 0.0)
    {

        int32_t RemovePosition = Display->text().size() - AllValues[CurrentValueIndex].size();
        Display->setText(Display->text().remove(RemovePosition, AllValues[CurrentValueIndex].size()));

        AllValues[CurrentValueIndex].prepend(tr("-"));

        Display->setText(Display->text() + AllValues[CurrentValueIndex]);
    }
    else /*if (AllValues[CurrentValueIndex].toDouble() < 0.0)*/
    {

        int32_t RemovePosition = Display->text().size() - AllValues[CurrentValueIndex].size();
        Display->setText(Display->text().remove(RemovePosition, AllValues[CurrentValueIndex].size()));
        AllValues[CurrentValueIndex].remove(0, 1);
        Display->setText(Display->text() + AllValues[CurrentValueIndex]);
    }
    CalculateAllOperation();
}

void Calculator::BackspaceClicked()
{
    QString Text = Display->text();
    if (AllValues.size() > AllOperations.size())
    {
        Text.chop(1);
        AllValues[CurrentValueIndex].chop(1);
        if (AllValues[CurrentValueIndex].isEmpty())
        {
            AllValues.removeLast();
        }
        if (AllValues.size() == 0)
        {
            Text = tr("0");
            DisplayTotal->clear();
            WaitingForValue = true;
        }
        // qDebug() << AllValues.size() << " == " << AllValues[CurrentValueIndex];
    }
    else
    {
        Text.chop(1);
        AllOperations.chop(1);
        std::max(--CurrentValueIndex, 0);
        WaitingForValue = true;
    }
    CalculateAllOperation();
    if (AllValues.size() == 1)
    {
        DisplayTotal->setText(AllValues[0]);
    }
    Display->setText(Text);
}

void Calculator::ClearClicked()
{
    // if (WaitingForValue) return;

    //    DisplayTotal->setText(tr("0"));
    //    WaitingForValue = true;
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
    DisplayTotal->setText(tr("error"));
}

Button* Calculator::GetButton(QObject* Sender) const
{
    return qobject_cast<Button*>(Sender);
}
