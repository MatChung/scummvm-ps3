/*
 * ps3-fs-factory.h
 *
 *  Created on: 05.11.2010
 *      Author: lousy
 */

#ifndef PS3FSFACTORY_H_
#define PS3FSFACTORY_H_

#include "common/singleton.h"
#include "backends/fs/fs-factory.h"

/**
 * Creates PS3FilesystemNode objects.
 *
 * Parts of this class are documented in the base interface class, AbstractFilesystemFactory.
 */
class Ps3FilesystemFactory : public FilesystemFactory, public Common::Singleton<Ps3FilesystemFactory> {
public:
	virtual AbstractFSNode *makeRootFileNode() const;
	virtual AbstractFSNode *makeCurrentDirectoryFileNode() const;
	virtual AbstractFSNode *makeFileNodePath(const Common::String &path) const;

//protected:
	Ps3FilesystemFactory();

private:
	friend class Common::Singleton<SingletonBaseType>;
};

#endif /* PS3FSFACTORY_H_ */
