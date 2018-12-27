#ifndef FILESLISTVIEWMODEL_H
#define FILESLISTVIEWMODEL_H

#include <QStringListModel>

struct FilesListViewModel : public QStringListModel
{

public:
  void append (const QString& string);

  FilesListViewModel &operator<<(const QString& string);

  Qt::ItemFlags flags (const QModelIndex& index) const;

};

#endif // FILESLISTVIEWMODEL_H
