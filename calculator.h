#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QKeyEvent>
#include <QLineEdit>
#include <QLabel>

class Calculator : public QWidget
{
    Q_OBJECT

public:
    explicit Calculator(QWidget *parent = nullptr);
    ~Calculator();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onButtonClicked();
    void onBackspace();
    void onClear();
    void onEqual();

private:
    void processInput(const QString &token);
    void inputDigit(const QString &digit);
    void inputPoint();
    void inputOperator(const QString &op);
    void calculatePending();
    void showError(const QString &msg);
    void updateHistory(const QString &text);

    QLineEdit *display;
    QLabel *historyDisplay; // 新增：历史记录标签

    double  m_storedOperand   = 0.0;
    QString m_pendingOperator;
    bool    m_waitingForOperand = true;
    bool    m_hasDecimal        = false;
    bool    m_hasError          = false;
};

#endif // CALCULATOR_H
