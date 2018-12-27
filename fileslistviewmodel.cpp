#include "fileslistviewmodel.h"

void FilesListViewModel::append(const QString &string){
    insertRows(rowCount(), 1);
    setData(index(rowCount()-1), string);
}

FilesListViewModel &FilesListViewModel::operator<<(const QString &string){
    append(string);
    return *this;
}

Qt::ItemFlags FilesListViewModel::flags (const QModelIndex & index) const {
    Qt::ItemFlags defaultFlags = QStringListModel::flags(index);
    if (index.isValid()){
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
    }
    return defaultFlags;
}
