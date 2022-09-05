#ifndef THEMES_H
#define THEMES_H

#include <QString>

class Theme
{
public:
    Theme(QString fileName);

    bool    Delete();
    virtual ~Theme();
    QString getId();
    QString getPath();
    bool getDeleteable();

    QString name() const;
    void setName(const QString &name);
    QString comment() const;
    void setComment(const QString &comment);
    QString example() const;
    void setExample(const QString &example);
    bool hasDark() const;
    void setHasDark(bool hasDark);

private:
    QString id;
    QString filePath;
    bool    deletable;

    QString m_name;
    QString m_comment;
    QString m_example;
    bool m_hasDark;
};

#endif // THEMES_H
