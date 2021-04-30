/*
    SPDX-FileCopyrightText: 2013 Reza Fatahilah Shah <rshah0385@kireihana.com>

    SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef SDDMKCM_H
#define SDDMKCM_H

#include <KQuickAddons/ManagedConfigModule>

class ThemeConfig;
class AdvancedConfig;
class SddmData;
class SddmSettings;
class ThemesModel;

class SddmKcm : public KQuickAddons::ManagedConfigModule
{
    Q_OBJECT
    Q_PROPERTY(SddmSettings *sddmSettings READ sddmSettings CONSTANT)
    Q_PROPERTY(ThemesModel *themesModel READ themesModel CONSTANT)
public:
    explicit SddmKcm(QObject *parent, const QVariantList &args);
    ~SddmKcm() override;

    Q_INVOKABLE static QString toLocalFile(const QUrl &url);
    Q_INVOKABLE void removeTheme(const QModelIndex &index);
    Q_INVOKABLE void installTheme(const QUrl &url);
    Q_INVOKABLE void synchronizeSettings();
    Q_INVOKABLE void resetSyncronizedSettings();

    SddmSettings *sddmSettings() const;
    ThemesModel *themesModel() const;
public Q_SLOTS:
    void save() override;
Q_SIGNALS:
    void errorOccured(const QString &message);

private:
    SddmData *m_data;
    ThemesModel *m_themesModel;
};

#endif // SDDMKCM_H
