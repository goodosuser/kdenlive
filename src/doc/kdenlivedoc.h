/***************************************************************************
 *   Copyright (C) 2007 by Jean-Baptiste Mardelle (jb@kdenlive.org)        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

/*! \class KdenliveDoc
    \brief Represents a kdenlive project file

   Instances of KdeliveDoc classes are created by void MainWindow::newFile(bool showProjectSettings, bool force)
*/
#ifndef KDENLIVEDOC_H
#define KDENLIVEDOC_H

#include <QDir>
#include <QList>
#include <QMap>
#include <QObject>
#include <QTimer>
#include <QUrl>
#include <QtXml/qdom.h>
#include <memory>

#include <KDirWatch>
#include <kautosavefile.h>

#include "definitions.h"
#include "gentime.h"
#include "mltcontroller/effectscontroller.h"
#include "timecode.h"
#include "timeline/guide.h"

class ClipManager;
class MainWindow;
class TrackInfo;
class ProjectClip;
class ClipController;
class MarkerListModel;
class Render;

class QTextEdit;
class QUndoGroup;
class QTimer;
class QUndoCommand;
class DocUndoStack;

namespace Mlt {
class Profile;
}

class KdenliveDoc : public QObject
{
    Q_OBJECT
public:
    KdenliveDoc(const QUrl &url, const QString &projectFolder, QUndoGroup *undoGroup, const QString &profileName, const QMap<QString, QString> &properties,
                const QMap<QString, QString> &metadata, const QPoint &tracks, bool *openBackup, MainWindow *parent = nullptr);
    ~KdenliveDoc();
    /** @brief Get current document's producer. */
    const QByteArray getProjectXml();
    QDomNodeList producersList();
    double fps() const;
    int width() const;
    int height() const;
    QUrl url() const;
    KAutoSaveFile *m_autosave;
    Timecode timecode() const;
    QDomDocument toXml();
    std::shared_ptr<DocUndoStack> commandStack();
    ClipManager *clipManager();
    QString groupsXml() const;

    void deleteClip(const QString &clipId, ClipType type, const QString &url);
    int getFramePos(const QString &duration);
    /** @brief Get a bin's clip from its id. */
    std::shared_ptr<ProjectClip> getBinClip(const QString &clipId);
    /** @brief Get a list of all clip ids that are inside a folder. */
    QStringList getBinFolderClipIds(const QString &folderId) const;

    /** @brief Returns current project profile. */
    MltVideoProfile mltProfile() const;
    ProfileInfo getProfileInfo() const;
    const QString description() const;
    void setUrl(const QUrl &url);

    /** @brief Defines whether the document needs to be saved. */
    bool isModified() const;

    /** @brief Returns the project folder, used to store project temporary files. */
    QString projectTempFolder() const;
    /** @brief Returns the folder used to store project data files (titles, etc). */
    QString projectDataFolder() const;
    void setZoom(int horizontal, int vertical);
    QPoint zoom() const;
    double dar() const;
    double projectDuration() const;
    /** @brief Returns the project file xml. */
    QDomDocument xmlSceneList(const QString &scene);
    /** @brief Saves the project file xml to a file. */
    bool saveSceneList(const QString &path, const QString &scene);
    /** @brief Saves only the MLT xml to a file for preview rendering. */
    void saveMltPlaylist(const QString &fileName);
    void cacheImage(const QString &fileId, const QImage &img) const;
    void setProjectFolder(const QUrl &url);
    void setZone(int start, int end);
    QPoint zone() const;
    void setDocumentProperty(const QString &name, const QString &value);
    const QString getDocumentProperty(const QString &name, const QString &defaultValue = QString()) const;

    /** @brief Gets the list of renderer properties saved into the document. */
    QMap<QString, QString> getRenderProperties() const;
    /** @brief Read the display ratio from an xml project file. */
    static double getDisplayRatio(const QString &path);
    /** @brief Backup the project file */
    void backupLastSavedVersion(const QString &path);
    /** @brief Returns the document metadata (author, copyright, ...) */
    const QMap<QString, QString> metadata() const;
    /** @brief Set the document metadata (author, copyright, ...) */
    void setMetadata(const QMap<QString, QString> &meta);
    /** @brief Add url to the file watcher so that we monitor changes */
    void watchFile(const QString &url);
    /** @brief Get all document properties that need to be saved */
    QMap<QString, QString> documentProperties();
    bool useProxy() const;
    bool autoGenerateProxy(int width) const;
    bool autoGenerateImageProxy(int width) const;
    /** @brief Saves effects embedded in project file. */
    void saveCustomEffects(const QDomNodeList &customeffects);
    void resetProfile();
    /** @brief Force processing of clip id in producer queue. */
    void forceProcessing(const QString &id);
    void getFileProperties(const QDomElement &xml, const QString &clipId, int imageHeight, bool replaceProducer = true);
    /** @brief Returns true if the profile file has changed. */
    bool profileChanged(const QString &profile) const;
    /** @brief Get an action from main actioncollection. */
    QAction *getAction(const QString &name);
    /** @brief Add an action to main actioncollection. */
    void doAddAction(const QString &name, QAction *a, const QKeySequence &shortcut);
    void invalidatePreviews(QList<int> chunks);
    void previewProgress(int p);
    /** @brief Select most appropriate rendering profile for timeline preview based on fps / size. */
    void selectPreviewProfile();
    void displayMessage(const QString &text, MessageType type = DefaultMessage, int timeOut = 0);
    /** @brief Get a cache directory for this project. */
    QDir getCacheDir(CacheType type, bool *ok) const;
    /** @brief Create standard cache dirs for the project */
    void initCacheDirs();
    /** @brief Get a list of all proxy hash used in this project */
    QStringList getProxyHashList();
    /** @brief Returns true if advanced compositing is available */
    static int compositingMode();
    /** @brief Move project data files to new url */
    void moveProjectData(const QString &src, const QString &dest);

    /** @brief Returns a pointer to the guide model */
    std::shared_ptr<MarkerListModel> getGuideModel() const;

    /** @brief Edit timeline guide */
    void addGuides(QList<CommentedTime> &markers);
    /** @brief Load bin thumbnails after document opening */
    void loadThumbs();
    /** @brief Get timeline guide at pos. ok set to false if no guide */
    CommentedTime getGuide(const GenTime &pos, bool *ok) const;

    // TODO REFAC: delete */
    Render *renderer();

private:
    QUrl m_url;
    QDomDocument m_document;
    KDirWatch m_fileWatcher;
    /** Timer used to reload clips when they have been externally modified */
    QTimer m_modifiedTimer;
    /** List of the clip IDs that need to be reloaded after being externally modified */
    QMap<QString, QTime> m_modifiedClips;
    Timecode m_timecode;
    std::shared_ptr<DocUndoStack> m_commandStack;
    ClipManager *m_clipManager;
    QString m_searchFolder;

    /** @brief Tells whether the current document has been changed after being saved. */
    bool m_modified;

    /** @brief The project folder, used to store project files (titles, effects...). */
    QString m_projectFolder;
    QList<int> m_undoChunks;
    QMap<QString, QString> m_documentProperties;
    QMap<QString, QString> m_documentMetadata;
    std::shared_ptr<MarkerListModel> m_guideModel;

    QString searchFileRecursively(const QDir &dir, const QString &matchSize, const QString &matchHash) const;

    /** @brief Creates a new project. */
    QDomDocument createEmptyDocument(int videotracks, int audiotracks);
    QDomDocument createEmptyDocument(const QList<TrackInfo> &tracks);

    /** @brief Updates the project folder location entry in the kdenlive file dialogs to point to the current project folder. */
    void updateProjectFolderPlacesEntry();
    /** @brief Only keep some backup files, delete some */
    void cleanupBackupFiles();
    /** @brief Load document properties from the xml file */
    void loadDocumentProperties();
    /** @brief update document properties to reflect a change in the current profile */
    void updateProjectProfile(bool reloadProducers = false);

public slots:
    void slotCreateTextTemplateClip(const QString &group, const QString &groupId, QUrl path);

    /** @brief Sets the document as modified or up to date.
     * @description  If crash recovery is turned on, a timer calls KdenliveDoc::slotAutoSave() \n
     * Emits docModified conected to MainWindow::slotUpdateDocumentState \n
     * @param mod (optional) true if the document has to be saved */
    void setModified(bool mod = true);
    void slotProxyCurrentItem(bool doProxy, QList<std::shared_ptr<ProjectClip>> clipList = QList<std::shared_ptr<ProjectClip>>(), bool force = false,
                              QUndoCommand *masterCommand = nullptr);
    /** @brief Saves the current project at the autosave location.
     * @description The autosave files are in ~/.kde/data/stalefiles/kdenlive/ */
    void slotAutoSave();

private slots:
    void slotClipModified(const QString &path);
    void slotClipMissing(const QString &path);
    void slotProcessModifiedClips();
    void slotModified();
    void switchProfile(MltVideoProfile profile, const QString &id, const QDomElement &xml);
    void slotSwitchProfile();
    /** @brief Check if we did a new action invalidating more recent undo items. */
    void checkPreviewStack();

signals:
    void resetProjectList();

    /** @brief Informs that the document status has been changed.
     *
     * If the document has been modified, it's called with true as an argument. */
    void docModified(bool);
    void selectLastAddedClip(const QString &);
    void guidesUpdated();
    /** @brief When creating a backup file, also save a thumbnail of current timeline */
    void saveTimelinePreview(const QString &path);
    /** @brief Trigger the autosave timer start */
    void startAutoSave();
    /** @brief Current doc created effects, reload list */
    void reloadEffects();
    /** @brief Fps was changed, update timeline (changed = 1 means no change) */
    void updateFps(double changed);
    /** @brief If a command is pushed when we are in the middle of undo stack, invalidate further undo history */
    void removeInvalidUndo(int ix);
    /** @brief Update compositing info */
    void updateCompositionMode(int);
};

#endif
