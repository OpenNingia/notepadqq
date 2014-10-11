#include "include/frmindentationmode.h"
#include "include/EditorNS/editor.h"
#include "ui_frmindentationmode.h"

frmIndentationMode::frmIndentationMode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::frmIndentationMode)
{
    ui->setupUi(this);
}

frmIndentationMode::~frmIndentationMode()
{
    delete ui;
}

EditorNS::Editor::IndentationMode frmIndentationMode::indentationMode()
{
    EditorNS::Editor::IndentationMode indent;
    indent.useTabs = !ui->chkLanguages_IndentWithSpaces->isChecked();
    indent.size = ui->txtLanguages_TabSize->value();
    return indent;
}

void frmIndentationMode::on_buttonBox_accepted()
{
    accept();
}

void frmIndentationMode::on_buttonBox_rejected()
{
    reject();
}