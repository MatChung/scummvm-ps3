/*
 * ps3-fs-node.h
 *
 *  Created on: 05.11.2010
 *      Author: lousy
 */

#ifndef PS3FSNODE_H_
#define PS3FSNODE_H_

#include "ps3-fs-factory.h"
/**
 * Implementation of the ScummVM file system API based on the Ps3SDK.
 *
 * Parts of this class are documented in the base interface class, AbstractFSNode.
 */
class Ps3FilesystemNode : public AbstractFSNode {

friend class Ps3FilesystemFactory;

protected:
	Common::String _displayName;
	Common::String _path;
	bool _isDirectory;
	//bool _isRoot;
	bool _isHere;
	bool _verified;

private:
	void doverify();

public:
	/**
	 * Creates a PS3FilesystemNode with the root node as path.
	 */
	Ps3FilesystemNode();

	/**
	 * Creates a PS3FilesystemNode for a given path.
	 *
	 * @param path Common::String with the path the new node should point to.
	 */
	Ps3FilesystemNode(const Common::String &path);
	Ps3FilesystemNode(const Common::String &path, bool verify);

	/**
	 * Copy constructor.
	 */
	Ps3FilesystemNode(const Ps3FilesystemNode *node);

	virtual Common::String getDisplayName() const { return _displayName; }
	virtual Common::String getName() const { return _displayName; }
	virtual Common::String getPath() const { return _path; }

	virtual bool exists() const {
		// printf("%s : is %d\n", _path.c_str(), _isHere);
		return _isHere;
	}

	virtual bool isDirectory() const {
		// printf("%s : dir %d\n", _path.c_str(), _isDirectory);
		return _isDirectory;
	}

	virtual bool isReadable() const {
		return _isHere;
	}

	virtual bool isWritable() const {
		return true; // exists(); // creating ?
	}

	virtual AbstractFSNode *clone() const { return new Ps3FilesystemNode(this); }
	virtual AbstractFSNode *getChild(const Common::String &n) const;
	virtual bool getChildren(AbstractFSList &list, ListMode mode, bool hidden) const;
	virtual AbstractFSNode *getParent() const;

	virtual Common::SeekableReadStream *createReadStream();
	virtual Common::WriteStream *createWriteStream();
};
#endif /* PS3FSNODE_H_ */
