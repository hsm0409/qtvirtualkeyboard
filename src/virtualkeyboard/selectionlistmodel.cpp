/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Virtual Keyboard module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtVirtualKeyboard/selectionlistmodel.h>
#include <QtVirtualKeyboard/abstractinputmethod.h>
#include <QtVirtualKeyboard/private/settings_p.h>
#include <QtCore/private/qabstractitemmodel_p.h>
#include <QtCore/qpointer.h>

QT_BEGIN_NAMESPACE

using namespace QtVirtualKeyboard;

class QVirtualKeyboardSelectionListModelPrivate : public QAbstractItemModelPrivate
{
public:
    QVirtualKeyboardSelectionListModelPrivate() :
        QAbstractItemModelPrivate(),
        dataSource(nullptr),
        type(QVirtualKeyboardSelectionListModel::WordCandidateList),
        rowCount(0),
        wclAutoCommitWord(false)
    {
    }

    QHash<int, QByteArray> roles;
    QPointer<QVirtualKeyboardAbstractInputMethod> dataSource;
    QVirtualKeyboardSelectionListModel::Type type;
    int rowCount;
    bool wclAutoCommitWord;
};

/*!
    \qmltype QVirtualKeyboardSelectionListModel
    \instantiates QVirtualKeyboardSelectionListModel
    \inqmlmodule QtQuick.VirtualKeyboard
    \ingroup qtvirtualkeyboard-qml
    \brief Provides a data model for the selection lists.

    The QVirtualKeyboardSelectionListModel is a data model for word candidates
    provided by the input method.

    An instance of QVirtualKeyboardSelectionListModel cannot be created directly.
    Instead, the InputEngine manages the instances and provides
    access to the model by InputEngine::wordCandidateListModel
    property.

    The model exposes the following data roles for the list delegate:
    \list
        \li \c display Display text for item
        \li \c wordCompletionLength Word completion length for item
    \endlist

    The activeItemChanged signal indicates which item is currently
    highlighted by the input method. The view should respond to this
    signal by highlighting the corresponding item in the list.

    The user selection is handled by the selectItem() method. The view
    should be invoke this method when the user selects an item from the
    list.
*/

/*!
    \class QVirtualKeyboardSelectionListModel

    \inmodule QtVirtualKeyboard

    \brief List model for selection lists.

    This class acts as a bridge between the UI and the
    input method that provides the data for selection
    lists.
*/

/*!
    \enum QVirtualKeyboardSelectionListModel::Type

    This enum specifies the type of selection list.

    \value WordCandidateList
           Shows list of word candidates
*/

/*!
    \enum QVirtualKeyboardSelectionListModel::Role

    This enum specifies a role of the data requested.

    \value DisplayRole
           The data to be rendered in form of text.
    \value WordCompletionLengthRole
           An integer specifying the length of the word
           the completion part expressed as the
           number of characters counted from the
           end of the string.
    \value DictionaryTypeRole
           An integer specifying \ l {QVirtualKeyboardSelectionListModel::DictionaryType}{dictionary type}.
    \value CanRemoveSuggestionRole
           A boolean value indicating if the word candidate
           can be removed from dictionary.
*/

/*!
    \enum QVirtualKeyboardSelectionListModel::DictionaryType

    This enum specifies the dictionary type of a word.

    \value DefaultDictionary
           The word candidate is from the default dictionary.
    \value UserDictionary
           The word candidate is from the user dictionary.
*/

QVirtualKeyboardSelectionListModel::QVirtualKeyboardSelectionListModel(QObject *parent) :
    QAbstractListModel(*new QVirtualKeyboardSelectionListModelPrivate(), parent)
{
    Q_D(QVirtualKeyboardSelectionListModel);
    d->roles[DisplayRole] = "display";
    d->roles[WordCompletionLengthRole] = "wordCompletionLength";
}

/*!
    \internal
*/
QVirtualKeyboardSelectionListModel::~QVirtualKeyboardSelectionListModel()
{
}

/*!
    \internal
*/
void QVirtualKeyboardSelectionListModel::setDataSource(QVirtualKeyboardAbstractInputMethod *dataSource, Type type)
{
    Q_D(QVirtualKeyboardSelectionListModel);
    if (d->dataSource) {
        disconnect(this, SLOT(selectionListChanged(int)));
        disconnect(this, SLOT(selectionListActiveItemChanged(int, int)));
    }
    d->type = type;
    if (d->dataSource) {
        d->dataSource = nullptr;
        selectionListChanged(type);
        selectionListActiveItemChanged(type, -1);
    }
    d->dataSource = dataSource;
    if (d->dataSource) {
        connect(d->dataSource, SIGNAL(selectionListChanged(int)), SLOT(selectionListChanged(int)));
        connect(d->dataSource, SIGNAL(selectionListActiveItemChanged(int, int)), SLOT(selectionListActiveItemChanged(int, int)));
    }
}

/*!
    \internal
*/
QVirtualKeyboardAbstractInputMethod *QVirtualKeyboardSelectionListModel::dataSource() const
{
    Q_D(const QVirtualKeyboardSelectionListModel);
    return d->dataSource;
}

/*!
    \internal
*/
int QVirtualKeyboardSelectionListModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const QVirtualKeyboardSelectionListModel);
    Q_UNUSED(parent)
    return d->rowCount;
}

/*!
    \internal
*/
QVariant QVirtualKeyboardSelectionListModel::data(const QModelIndex &index, int role) const
{
    Q_D(const QVirtualKeyboardSelectionListModel);
    return d->dataSource ? d->dataSource->selectionListData(d->type, index.row(), role) : QVariant();
}

/*!
    \internal
*/
QHash<int,QByteArray> QVirtualKeyboardSelectionListModel::roleNames() const
{
    Q_D(const QVirtualKeyboardSelectionListModel);
    return d->roles;
}

/*!
    \internal
*/
int QVirtualKeyboardSelectionListModel::count() const
{
    Q_D(const QVirtualKeyboardSelectionListModel);
    return d->rowCount;
}

/*! \qmlmethod void QVirtualKeyboardSelectionListModel::selectItem(int index)

    This method should be called when the user selects an item at position
    \a index from the list.
    The selection is forwarded to the input method for further processing.
*/
/*!
    \fn void QVirtualKeyboardSelectionListModel::selectItem(int index)

    This method should be called when the user selects an item at position
    \a index from the list.
    The selection is forwarded to the input method for further processing.
*/
void QVirtualKeyboardSelectionListModel::selectItem(int index)
{
    Q_D(QVirtualKeyboardSelectionListModel);
    if (index >= 0 && index < d->rowCount && d->dataSource) {
        emit itemSelected(index);
        d->dataSource->selectionListItemSelected(d->type, index);
    }
}

void QVirtualKeyboardSelectionListModel::removeItem(int index)
{
    Q_D(QVirtualKeyboardSelectionListModel);
    if (index >= 0 && index < d->rowCount && d->dataSource) {
        d->dataSource->selectionListRemoveItem(d->type, index);
    }
}

/*!
 * \internal
 */
QVariant QVirtualKeyboardSelectionListModel::dataAt(int index, int role) const
{
    return data(this->index(index, 0), role);
}

/*!
    \internal
*/
void QVirtualKeyboardSelectionListModel::selectionListChanged(int type)
{
    Q_D(QVirtualKeyboardSelectionListModel);
    if (static_cast<Type>(type) == d->type) {
        int oldCount = d->rowCount;
        int newCount = d->dataSource ? d->dataSource->selectionListItemCount(d->type) : 0;
        if (newCount) {
            int changedCount = qMin(oldCount, newCount);
            if (changedCount)
                emit dataChanged(index(0), index(changedCount - 1));
            if (oldCount > newCount) {
                beginRemoveRows(QModelIndex(), newCount, oldCount - 1);
                d->rowCount = newCount;
                endRemoveRows();
            } else if (oldCount < newCount) {
                beginInsertRows(QModelIndex(), oldCount, newCount - 1);
                d->rowCount = newCount;
                endInsertRows();
            }
        } else {
            beginResetModel();
            d->rowCount = 0;
            endResetModel();
        }
        if (type == QVirtualKeyboardSelectionListModel::WordCandidateList)
            d->wclAutoCommitWord = ((oldCount > 1 || (oldCount == 1 && d->wclAutoCommitWord)) && newCount == 1 &&
                                 Settings::instance()->wclAutoCommitWord() &&
                                 dataAt(0).toString().length() > 1);
        if (d->rowCount != oldCount)
            emit countChanged();
    }
}

/*!
    \internal
*/
void QVirtualKeyboardSelectionListModel::selectionListActiveItemChanged(int type, int index)
{
    Q_D(QVirtualKeyboardSelectionListModel);
    if (static_cast<Type>(type) == d->type && index < d->rowCount) {
        emit activeItemChanged(index);
        if (index == 0 && d->wclAutoCommitWord)
            selectItem(0);
    }
}

/*!
    \qmlsignal void QVirtualKeyboardSelectionListModel::activeItemChanged(int index)

    This signal is emitted when the active item in the list changes. The
    UI should react to this signal by highlighting the item at \a index in
    the list.
*/
/*!
    \fn void QVirtualKeyboardSelectionListModel::activeItemChanged(int index)

    This signal is emitted when the active item in the list changes. The
    UI should react to this signal by highlighting the item at \a index in
    the list.
*/

/*!
    \qmlsignal void QVirtualKeyboardSelectionListModel::itemSelected(int index)

    This signal is emitted when an item at \a index is selected by the user.
*/
/*!
    \fn void QVirtualKeyboardSelectionListModel::itemSelected(int index)

    This signal is emitted when an item at \a index is selected by the user.
*/

QT_END_NAMESPACE
