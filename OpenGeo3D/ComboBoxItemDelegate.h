#pragma once

#include <QtWidgets/QStyledItemDelegate>

class ComboBoxItemDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:
	ComboBoxItemDelegate(QObject* parent);
	virtual ~ComboBoxItemDelegate();

	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

	void setValues(const QStringList& values);

private:
	QStringList options_;
};

