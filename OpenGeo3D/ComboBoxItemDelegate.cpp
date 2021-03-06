#include "ComboBoxItemDelegate.h"
#include <QtWidgets/QComboBox>

ComboBoxItemDelegate::ComboBoxItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {

}

ComboBoxItemDelegate::~ComboBoxItemDelegate() {

}

void ComboBoxItemDelegate::setValues(const QStringList& values) {
	options_ = values;
}

QWidget* ComboBoxItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	if (index.isValid()) {
		QComboBox* comboBox = new QComboBox(parent);
		comboBox->addItems(options_);
		return comboBox;
	} else {
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

void ComboBoxItemDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
	if (index.isValid()) {
		QComboBox* comboBox = dynamic_cast<QComboBox*>(editor);
		if (comboBox != nullptr) {
			QString text = index.model()->data(index, Qt::DisplayRole).toString();
			comboBox->setCurrentText(text);
			return;
		}
	}
	QStyledItemDelegate::setEditorData(editor, index);
}

void ComboBoxItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
	if (index.isValid()) {
		QComboBox* comboBox = dynamic_cast<QComboBox*>(editor);
		if (comboBox != nullptr) {
			QString value = comboBox->currentText();
			model->setData(index, value, Qt::DisplayRole);
		}

	}
	QStyledItemDelegate::setModelData(editor, model, index);
}
