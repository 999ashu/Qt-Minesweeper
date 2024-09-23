#include "multibutton.h"

MultiButton::MultiButton(QWidget *parent, bool *Inversion) : QPushButton(parent), Inversion_(Inversion) {}

void MultiButton::mousePressEvent(QMouseEvent *Event)
{
	if (Event->button() == Qt::LeftButton)
	{
		emit *Inversion_ ? rightClicked() : leftClicked();
	}
	else if (Event->button() == Qt::RightButton)
	{
		emit *Inversion_ ? leftClicked() : rightClicked();
	}
	else if (Event->button() == Qt::MiddleButton)
	{
		emit middleClicked();
	}
}
