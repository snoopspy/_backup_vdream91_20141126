#ifndef VMYOBJ_H
#define VMYOBJ_H

#include <QObject>

class VMyObj : public QObject
{
	Q_OBJECT
public:
	explicit VMyObj(QObject *parent = 0);

signals:

public slots:

};

#endif // VMYOBJ_H
