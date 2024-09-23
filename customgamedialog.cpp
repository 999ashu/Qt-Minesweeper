#include "customgamedialog.h"

CustomGameDialog::CustomGameDialog(QWidget *parent) : QDialog(parent)
{
	QVBoxLayout *Layout = new QVBoxLayout(this);

	QLabel *RowsLabel = new QLabel(tr("Rows:"), this);
	RowsBox_ = new QSpinBox(this);
	RowsBox_->setRange(2, 40);
	RowsBox_->setValue(10);

	QLabel *ColumnsLabel = new QLabel(tr("Columns:"), this);
	ColumnBox_ = new QSpinBox(this);
	ColumnBox_->setRange(2, 40);
	ColumnBox_->setValue(10);

	QLabel *MinesLabel = new QLabel(tr("Mines:"), this);
	MinesBox_ = new QSpinBox(this);

	MinesBox_->setRange(2, RowsBox_->value() * ColumnBox_->value() - 2);
	MinesBox_->setValue(10);

	QPushButton *OkButton = new QPushButton(tr("Ok"), this);
	QPushButton *CancelButton = new QPushButton(tr("Cancel"), this);

	ButtonBox_ = new QDialogButtonBox(this);
	ButtonBox_->addButton(OkButton, QDialogButtonBox::AcceptRole);
	ButtonBox_->addButton(CancelButton, QDialogButtonBox::RejectRole);

	Layout->addWidget(RowsLabel);
	Layout->addWidget(RowsBox_);
	Layout->addWidget(ColumnsLabel);
	Layout->addWidget(ColumnBox_);
	Layout->addWidget(MinesLabel);
	Layout->addWidget(MinesBox_);
	Layout->addWidget(ButtonBox_);

	connect(RowsBox_, QOverload< int >::of(&QSpinBox::valueChanged), this, &CustomGameDialog::updateRange);
	connect(MinesBox_, QOverload< int >::of(&QSpinBox::valueChanged), this, &CustomGameDialog::updateRange);
	connect(OkButton, &QPushButton::clicked, this, &QDialog::accept);
	connect(CancelButton, &QPushButton::clicked, this, &QDialog::reject);

	setFixedSize(sizeHint());
}

void CustomGameDialog::updateRange()
{
	MinesBox_->setRange(1, RowsBox_->value() * ColumnBox_->value() - 2);
}

int CustomGameDialog::getRows() const
{
	return RowsBox_->value();
}

int CustomGameDialog::getColumns() const
{
	return ColumnBox_->value();
}

int CustomGameDialog::getMines() const
{
	return MinesBox_->value();
}
