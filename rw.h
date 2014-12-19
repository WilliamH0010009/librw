namespace Rw {

struct Object
{
	uint8 type;
	uint8 subType;
	uint8 flags;
	void *parent;
};

struct Texture : PluginBase<Texture>
{
	char name[32];
	char mask[32];
	uint32 filterAddressing;

	Texture(void);
	Texture(Texture *t);
	~Texture(void);
	static Texture *streamRead(std::istream &stream);
	bool streamWrite(std::ostream &stream);
	uint32 streamGetSize(void);
};

struct Material : PluginBase<Material>
{
	Texture *texture;
	uint8 color[4];
	float32 surfaceProps[3];

	Material(void);
	Material(Material *m);
	~Material(void);
	static Material *streamRead(std::istream &stream);
	bool streamWrite(std::ostream &stream);
	uint32 streamGetSize(void);
};

struct MorphTarget
{
	float32 boundingSphere[4];
	float32 *vertices;
	float32 *normals;
};

struct Geometry : PluginBase<Geometry>, Object
{
	uint32 geoflags;
	int32 numTriangles;
	int32 numVertices;
	int32 numMorphTargets;
	int32 numTexCoordSets;

	uint16 *triangles;
	uint8 *colors;
	float32 *texCoords[8];

	MorphTarget *morphTargets;

	int32 numMaterials;
	Material **materialList;

	Geometry(void);
	Geometry(Geometry *g);
	~Geometry(void);
	static Geometry *streamRead(std::istream &stream);
	bool streamWrite(std::ostream &stream);
	uint32 streamGetSize(void);

	enum Flags
	{
		TRISTRIP  = 0x01,
		POSITIONS = 0x02,
		TEXTURED  = 0x04,
		PRELIT    = 0x08,
		NORMALS   = 0x10,
		LIGHT     = 0x20,
		MODULATE  = 0x40,
		TEXTURED2 = 0x80,
		NATIVE         = 0x01000000,
		NATIVEINSTANCE = 0x02000000
	};
};

struct Frame : PluginBase<Frame>, Object
{
	typedef Frame *(*Callback)(Frame *f, void *data);
	float32	matrix[16];
	float32	ltm[16];

	Frame *child;
	Frame *next;
	Frame *root;

	// temporary
	int32 matflag;

	Frame(void);
	Frame(Frame *f);
	~Frame(void);
	Frame *addChild(Frame *f);
	Frame *forAllChildren(Callback cb, void *data);
	int32 count(void);
};

struct Clump;

struct Light : PluginBase<Light>, Object
{
	Frame *frame;
	float32 radius;
	float32 color[4];
	float32 minusCosAngle;
	Clump *clump;

	Light(void);
	Light(Light *l);
	~Light(void);
	static Light *streamRead(std::istream &stream);
	bool streamWrite(std::ostream &stream);
	uint32 streamGetSize(void);
};

struct Atomic : PluginBase<Atomic>, Object
{
	Frame *frame;
	Geometry *geometry;
	Clump *clump;

	Atomic(void);
	Atomic(Atomic *a);
	~Atomic(void);
	static Atomic *streamReadClump(std::istream &stream,
		Frame **frameList, Geometry **geometryList);
	bool streamWriteClump(std::ostream &stream,
		Frame **frameList, int32 numFrames);
	uint32 streamGetSize(void);
};

struct Clump : PluginBase<Clump>, Object
{
	int32 numAtomics;
	Atomic **atomicList;
	int32 numLights;
	Light **lightList;
	int32 numCameras;
	// cameras not implemented

	Clump(void);
	Clump(Clump *c);
	~Clump(void);
	static Clump *streamRead(std::istream &stream);
	bool streamWrite(std::ostream &stream);
	uint32 streamGetSize(void);

private:
	void frameListStreamRead(std::istream &stream, Frame ***flp, int32 *nf);
	void frameListStreamWrite(std::ostream &stream, Frame **flp, int32 nf);
};

}