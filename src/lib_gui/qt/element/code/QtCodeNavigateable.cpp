#include "QtCodeNavigateable.h"

#include <QPropertyAnimation>
#include <QScrollArea>
#include <QScrollBar>

#include "ApplicationSettings.h"

QtCodeNavigateable::~QtCodeNavigateable()
{
}

void QtCodeNavigateable::ensureWidgetVisibleAnimated(
	const QWidget* parentWidget, const QWidget *childWidget, QRectF rect, bool animated, CodeScrollParams::Target target)
{
	QAbstractScrollArea* area = getScrollArea();
	if (!area || !parentWidget->isAncestorOf(childWidget))
	{
		return;
	}

	QRect focusRect = getFocusRectForWidget(childWidget, parentWidget);
	const QRect visibleRect(-parentWidget->pos(), area->viewport()->size());

	if (rect.height() > 0)
	{
		focusRect = QRect(childWidget->mapTo(parentWidget, rect.topLeft().toPoint()), rect.size().toSize());

		if (focusRect.height() < 100)
		{
			focusRect.adjust(0, 0, 0, 100);
		}
	}


	// scroll to top if widget is bigger than view
	if (focusRect.height() > visibleRect.height())
	{
		target = CodeScrollParams::Target::TOP;
	}

	int value = 0;
	switch (target)
	{
	case CodeScrollParams::Target::VISIBLE:
		if (focusRect.top() > visibleRect.top() && focusRect.bottom() < visibleRect.bottom())
		{
			return;
		}
		else if (focusRect.top() < visibleRect.top())
		{
			value = focusRect.top() - visibleRect.top() - 50;
		}
		else
		{
			value = focusRect.bottom() - visibleRect.bottom() + 50;
		}
		break;

	case CodeScrollParams::Target::CENTER:
		value = focusRect.center().y() - visibleRect.center().y();
		if (abs(value) < 50)
		{
			value = 0;
		}
		break;

	case CodeScrollParams::Target::TOP:
		value = focusRect.top() - visibleRect.top() - 50;
		if (value > 0 && value < 150)
		{
			value = 0;
		}
		break;
	}

	QScrollBar* scrollBar = area->verticalScrollBar();
	if (scrollBar && value)
	{
		if (animated && ApplicationSettings::getInstance()->getUseAnimations() && area->isVisible())
		{
			QPropertyAnimation* anim = new QPropertyAnimation(scrollBar, "value");
			anim->setDuration(300);
			anim->setStartValue(scrollBar->value());
			anim->setEndValue(scrollBar->value() + value);
			anim->setEasingCurve(QEasingCurve::InOutQuad);
			anim->start();
		}
		else
		{
			scrollBar->setValue(scrollBar->value() + value);
		}
	}
}

void QtCodeNavigateable::ensurePercentVisibleAnimated(
	double percentA, double percentB, bool animated, CodeScrollParams::Target target)
{
	QAbstractScrollArea* area = getScrollArea();
	if (!area)
	{
		return;
	}

	int totalHeight = area->sizeHint().height();
	int visibleHeight = area->viewport()->height();
	int scrollableHeight = totalHeight - visibleHeight;
	if (scrollableHeight < 0)
	{
		return;
	}

	QScrollBar* scrollBar = area->verticalScrollBar();
	double scrollFactor = double(scrollBar->maximum()) / scrollableHeight;

	int visibleY = double(scrollBar->value()) / scrollFactor;
	int scrollY = totalHeight * percentA;
	int rectHeight = percentB ? (totalHeight * percentB) - scrollY : 0;

	if (rectHeight > visibleHeight)
	{
		target = CodeScrollParams::Target::TOP;
	}

	switch (target)
	{
	case CodeScrollParams::Target::VISIBLE:
		if (scrollY > visibleY && scrollY + rectHeight < visibleY + scrollableHeight)
		{
			return;
		}

	case CodeScrollParams::Target::CENTER:
		if (rectHeight < visibleHeight / 2)
		{
			scrollY -= visibleHeight / 4;
		}
		else
		{
			scrollY += rectHeight / 2 - visibleHeight / 2;
		}
		break;

	case CodeScrollParams::Target::TOP:
		scrollY -= 100;
		break;
	}

	int value = scrollY * scrollFactor;
	int diff = value - scrollBar->value();
	if (diff > 5 || diff < -5)
	{
		if (animated && ApplicationSettings::getInstance()->getUseAnimations() && area->isVisible())
		{
			QPropertyAnimation* anim = new QPropertyAnimation(scrollBar, "value");
			anim->setDuration(300);
			anim->setStartValue(scrollBar->value());
			anim->setEndValue(value);
			anim->setEasingCurve(QEasingCurve::InOutQuad);
			anim->start();
		}
		else
		{
			scrollBar->setValue(value);
		}
	}
}

QRect QtCodeNavigateable::getFocusRectForWidget(const QWidget* childWidget, const QWidget* parentWidget) const
{
	const QRect microFocus = childWidget->inputMethodQuery(Qt::ImCursorRectangle).toRect();
	const QRect defaultMicroFocus = childWidget->QWidget::inputMethodQuery(Qt::ImCursorRectangle).toRect();

	if (microFocus != defaultMicroFocus)
	{
		return QRect(childWidget->mapTo(parentWidget, microFocus.topLeft()), microFocus.size());
	}
	else
	{
		return QRect(childWidget->mapTo(parentWidget, QPoint(0, 0)), childWidget->size());
	}
}
