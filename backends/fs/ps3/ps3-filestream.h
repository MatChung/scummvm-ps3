#include "common/scummsys.h"
#include "common/noncopyable.h"
#include "common/stream.h"


class PS3FileStream : public Common::SeekableReadStream, public Common::WriteStream, public Common::NonCopyable {
protected:
	/** File handle to the actual file. */
	int _fd;
	int _err;
	bool _eos;

public:
	/**
	 * Given a path, invokes fopen on that path and wrap the result in a
	 * PS3FileStream instance.
	 */
	static PS3FileStream *makeFromPath(const Common::String &path, bool writeMode);

	PS3FileStream(int fd);
	virtual ~PS3FileStream();

	virtual bool err() const;
	virtual void clearErr();
	virtual bool eos() const;

	virtual uint32 write(const void *dataPtr, uint32 dataSize);
	virtual bool flush();

	virtual int32 pos() const;
	virtual int32 size() const;
	virtual bool seek(int32 offs, int whence = SEEK_SET);
	virtual uint32 read(void *dataPtr, uint32 dataSize);
};
