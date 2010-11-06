#include "ps3-fs-factory.h"
#include "ps3-fs-node.h"


DECLARE_SINGLETON(Ps3FilesystemFactory)

Ps3FilesystemFactory::Ps3FilesystemFactory()
{
//	printf("Ps3FilesystemFactory::Ps3FilesystemFactory()\n");
}

AbstractFSNode *Ps3FilesystemFactory::makeRootFileNode() const {
//	printf("Ps3FilesystemFactory::makeRootFileNode()\n");
	return new Ps3FilesystemNode("/");
}

AbstractFSNode *Ps3FilesystemFactory::makeCurrentDirectoryFileNode() const {
//	printf("Ps3FilesystemFactory::makeCurrentDirectoryFileNode()\n");
	return new Ps3FilesystemNode();
}

AbstractFSNode *Ps3FilesystemFactory::makeFileNodePath(const Common::String &path) const {
//	printf("Ps3FilesystemFactory::makeFileNodePath(%s)\n",path.c_str());
	return new Ps3FilesystemNode(path, true);
}
