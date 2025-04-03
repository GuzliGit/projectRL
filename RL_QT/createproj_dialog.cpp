#include "createproj_dialog.h"
#include "ui_createproj_dialog.h"
#include <QMessageBox>
#include <QSizePolicy>

CreateProj_Dialog::CreateProj_Dialog(int block_limit, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CreateProj_Dialog)
{
    ui->setupUi(this);

    width = -1;
    height = -1;
    limit = block_limit;

    set_parameters();
}

CreateProj_Dialog::~CreateProj_Dialog()
{
    delete ui;
}

void CreateProj_Dialog::set_parameters()
{
    connect(ui->width_edit, &QPlainTextEdit::textChanged, this, &CreateProj_Dialog::on_text_changed);
    connect(ui->height_edit, &QPlainTextEdit::textChanged, this, &CreateProj_Dialog::on_text_changed);
}

int CreateProj_Dialog::get_width()
{
    return ui->width_edit->toPlainText().toInt();
}

int CreateProj_Dialog::get_height()
{
    return ui->height_edit->toPlainText().toInt();
}

bool CreateProj_Dialog::validate_accept()
{
    bool isint1, isint2;
    int val1 = ui->width_edit->toPlainText().toInt(&isint1);
    int val2 = ui->height_edit->toPlainText().toInt(&isint2);

    if (!isint1 || !isint2)
    {
        QMessageBox::warning(this, "Ошибка", "Числа должны быть целыми и неотрицательными!");
        return false;
    }

    if (val1 > limit || val2 > limit)
    {
        QMessageBox::warning(this, "Ошибка", "Числа не должны превышать " + QString::number(limit) + "!");
        return false;
    }

    return true;
}

void CreateProj_Dialog::on_decline_button_clicked()
{
    this->reject();
}


void CreateProj_Dialog::on_accept_button_clicked()
{
    if (validate_accept())
        this->accept();
}

void CreateProj_Dialog::on_text_changed()
{
    int temp = limit;
    int len_limit = 0;
    while (temp > 0) {
        len_limit++;
        temp /= 10;
    }

    QPlainTextEdit *edit = qobject_cast<QPlainTextEdit*>(sender());
    if (edit->toPlainText().length() > len_limit) {
        QString text = edit->toPlainText();
        text.chop(text.length() - len_limit);
        edit->setPlainText(text);

        QTextCursor cursor = edit->textCursor();
        cursor.movePosition(QTextCursor::End);
        edit->setTextCursor(cursor);
    }
}
