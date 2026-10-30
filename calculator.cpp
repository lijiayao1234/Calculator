#include "calculator.h"
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QtMath>
#include <QFont>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
{
    this->resize(320, 480);
    this->setWindowTitle("Calculator");
    this->setStyleSheet("QWidget { background-color: #FAFAFA; }");

    QWidget *displayArea = new QWidget(this);
    displayArea->setStyleSheet("background-color: #FFFFFF; border-bottom: 1px solid #E0E0E0;");

    historyDisplay = new QLabel("", displayArea);
    historyDisplay->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    historyDisplay->setStyleSheet("color: #888888; font-size: 16px; border: none; background: transparent;");

    display = new QLineEdit(displayArea);
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    display->setText("0");
    QFont font = display->font();
    font.setPointSize(40);
    font.setBold(true);
    display->setFont(font);
    display->setStyleSheet("QLineEdit { border: none; background-color: transparent; color: #222222; }");

    QVBoxLayout *displayLayout = new QVBoxLayout(displayArea);
    displayLayout->setContentsMargins(15, 15, 15, 15);
    displayLayout->setSpacing(5);
    displayLayout->addWidget(historyDisplay);
    displayLayout->addWidget(display);


    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(8);
    grid->setContentsMargins(10, 15, 10, 10);

    QString numStyle = "QPushButton { background-color: #F5F5F5; border-radius: 12px; font-size: 24px; color: #333; border: none; } QPushButton:pressed { background-color: #E0E0E0; }";
    QString funcStyle = "QPushButton { background-color: #E0F3FF; border-radius: 12px; font-size: 24px; color: #0088FF; border: none; } QPushButton:pressed { background-color: #C0E8FF; }";
    QString eqStyle = "QPushButton { background-color: #00A6FF; border-radius: 12px; font-size: 32px; color: white; border: none; } QPushButton:pressed { background-color: #0088DD; }";

    auto createBtn = [this, &grid](const QString &text, const QString &name, int row, int col, int rowSpan, int colSpan, const QString &style) {
        QPushButton *btn = new QPushButton(text, this);
        btn->setObjectName(name);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        btn->setStyleSheet(style);
        grid->addWidget(btn, row, col, rowSpan, colSpan);
        connect(btn, &QPushButton::clicked, this, &Calculator::onButtonClicked);
    };

    createBtn("C", "btnClear", 0, 0, 1, 1, funcStyle);
    createBtn("÷", "btnDiv",   0, 1, 1, 1, funcStyle);
    createBtn("×", "btnMul",   0, 2, 1, 1, funcStyle);
    createBtn("⌫", "btnBack",  0, 3, 1, 1, funcStyle);

    createBtn("7", "btn7",     1, 0, 1, 1, numStyle);
    createBtn("8", "btn8",     1, 1, 1, 1, numStyle);
    createBtn("9", "btn9",     1, 2, 1, 1, numStyle);
    createBtn("-", "btnSub",   1, 3, 1, 1, funcStyle);

    createBtn("4", "btn4",     2, 0, 1, 1, numStyle);
    createBtn("5", "btn5",     2, 1, 1, 1, numStyle);
    createBtn("6", "btn6",     2, 2, 1, 1, numStyle);
    createBtn("+", "btnAdd",   2, 3, 1, 1, funcStyle);

    createBtn("1", "btn1",     3, 0, 1, 1, numStyle);
    createBtn("2", "btn2",     3, 1, 1, 1, numStyle);
    createBtn("3", "btn3",     3, 2, 1, 1, numStyle);
    createBtn("=", "btnEqual", 3, 3, 2, 1, eqStyle);


    createBtn("±", "btnSign",  4, 0, 1, 1, numStyle);
    createBtn("0", "btn0",     4, 1, 1, 1, numStyle);
    createBtn(".", "btnPoint", 4, 2, 1, 1, numStyle);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(displayArea);
    mainLayout->addLayout(grid);
    this->setLayout(mainLayout);

    setFocusPolicy(Qt::StrongFocus);
}

Calculator::~Calculator() {}

void Calculator::updateHistory(const QString &text)
{
    historyDisplay->setText(text);
}

void Calculator::onButtonClicked()
{
    auto *btn = qobject_cast<QPushButton *>(sender());
    if (!btn) return;

    const QString name = btn->objectName();
    if (name.startsWith("btn") && name.size() == 4 && name.at(3).isDigit()) {
        processInput(name.mid(3));
    } else if (name == "btnPoint")   processInput(".");
    else if (name == "btnAdd")       processInput("+");
    else if (name == "btnSub")       processInput("-");
    else if (name == "btnMul")       processInput("*");
    else if (name == "btnDiv")       processInput("/");
    else if (name == "btnSign")      processInput("±");
    else if (name == "btnEqual")     processInput("=");
    else if (name == "btnBack")      processInput("BACK");
    else if (name == "btnClear")     processInput("CLEAR");
}

void Calculator::processInput(const QString &token)
{
    if (token.size() == 1 && token.at(0).isDigit()) {
        inputDigit(token);
    } else if (token == ".") {
        inputPoint();
    } else if (token == "+" || token == "-" || token == "*" || token == "/") {
        inputOperator(token);
    } else if (token == "±") {
        // 正负号处理逻辑
        QString cur = display->text();
        if (cur == "0" || cur == "0.") return;
        if (cur.startsWith("-")) {
            display->setText(cur.mid(1));
        } else {
            display->setText("-" + cur);
        }
        m_waitingForOperand = false;
    } else if (token == "=") {
        onEqual();
    } else if (token == "BACK") {
        onBackspace();
    } else if (token == "CLEAR") {
        onClear();
    }
}

void Calculator::inputDigit(const QString &digit)
{
    if (m_hasError) {
        onClear();
    }
    if (m_waitingForOperand) {
        display->setText(digit);
        m_waitingForOperand = false;
        m_hasDecimal = false;
        return;
    }
    QString cur = display->text();
    if (cur == "0") display->setText(digit);
    else            display->setText(cur + digit);
}

void Calculator::inputPoint()
{
    if (m_hasError) { onClear(); }
    if (m_waitingForOperand) {
        display->setText("0.");
        m_waitingForOperand = false;
        m_hasDecimal = true;
        return;
    }
    if (m_hasDecimal) return;
    display->setText(display->text() + ".");
    m_hasDecimal = true;
}

void Calculator::inputOperator(const QString &op)
{
    if (m_hasError) return;

    QString displayOp = op;
    if (op == "*") displayOp = "×";
    if (op == "/") displayOp = "÷";

    if (!m_pendingOperator.isEmpty() && !m_waitingForOperand) {
        calculatePending();
        if (m_hasError) return;
        updateHistory(display->text() + " " + displayOp);
    } else if (m_pendingOperator.isEmpty()) {
        m_storedOperand = display->text().toDouble();
        updateHistory(display->text() + " " + displayOp);
    }

    m_pendingOperator = op;
    m_waitingForOperand = true;
    m_hasDecimal = false;
}

void Calculator::onEqual()
{
    if (m_hasError) return;
    if (m_pendingOperator.isEmpty() || m_waitingForOperand) return;

    QString rightStr = display->text();
    QString displayOp = m_pendingOperator;
    if (displayOp == "*") displayOp = "×";
    if (displayOp == "/") displayOp = "÷";

    QString fullExpression = QString::number(m_storedOperand, 'g', 12) + " " + displayOp + " " + rightStr + " =";
    updateHistory(fullExpression);

    calculatePending();
    m_pendingOperator.clear();
    m_waitingForOperand = true;
    m_hasDecimal = false;
}

void Calculator::calculatePending()
{
    const double left  = m_storedOperand;
    const double right = display->text().toDouble();
    double result = 0.0;

    if (m_pendingOperator == "+") {
        result = left + right;
    } else if (m_pendingOperator == "-") {
        result = left - right;
    } else if (m_pendingOperator == "*") {
        result = left * right;
    } else if (m_pendingOperator == "/") {
        if (qFuzzyIsNull(right)) {
            showError("除数不能为零");
            return;
        }
        result = left / right;
    }

    m_storedOperand = result;
    display->setText(QString::number(result, 'g', 12));
}

void Calculator::onBackspace()
{
    if (m_hasError) { onClear(); return; }
    if (m_waitingForOperand) return;
    QString text = display->text();
    if (text.endsWith('.')) m_hasDecimal = false;
    text.chop(1);
    if (text.isEmpty() || text == "-") text = "0";
    display->setText(text);
}

void Calculator::onClear()
{
    display->setText("0");
    updateHistory("");
    m_storedOperand = 0.0;
    m_pendingOperator.clear();
    m_waitingForOperand = true;
    m_hasDecimal = false;
    m_hasError = false;
}

void Calculator::showError(const QString &msg)
{
    display->setText(msg);
    m_hasError = true;
    m_pendingOperator.clear();
    m_waitingForOperand = true;
    m_hasDecimal = false;
}

void Calculator::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        processInput(QString(QChar('0' + (key - Qt::Key_0))));
        event->accept();
        return;
    }
    switch (key) {
    case Qt::Key_Period: case Qt::Key_Comma: processInput("."); break;
    case Qt::Key_Plus: processInput("+"); break;
    case Qt::Key_Minus: processInput("-"); break;
    case Qt::Key_Asterisk: processInput("*"); break;
    case Qt::Key_Slash: processInput("/"); break;
    case Qt::Key_Return: case Qt::Key_Enter: case Qt::Key_Equal: processInput("="); break;
    case Qt::Key_Backspace: processInput("BACK"); break;
    case Qt::Key_Escape: case Qt::Key_Delete: processInput("CLEAR"); break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }
    event->accept();
}
