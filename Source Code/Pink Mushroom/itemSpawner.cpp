#include <common.h>
#include <game.h>
#include <profile.h>

const char *ItemSpawnerList[] = {"I_hammer", 0};

class daItemSpawner : public dEn_c {
    public:
        int onCreate();
        int onDelete();
        int onExecute();
        int onDraw();

        mHeapAllocator_c allocator;
	    nw4r::g3d::ResFile resFile;

	    m3d::mdl_c model;
        m3d::anmChr_c chrAnimation;
        ActivePhysics::Info HitMeBaby;

        void updateModelMatrices();

        void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
        bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);

        void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

        static dActor_c *build();
};

dActor_c *daItemSpawner::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daItemSpawner));
	daItemSpawner *c = new(buffer) daItemSpawner;

	return c;
}

const SpriteData daItemSpawnerSpriteData = { ProfileId::ItemSpawner, 0, 0, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile daItemSpawnerProfile(&daItemSpawner::build, SpriteId::ItemSpawner, &daItemSpawnerSpriteData, ProfileId::ItemSpawner, ProfileId::ItemSpawner, "ItemSpawner", ItemSpawnerList, 0x80);

int daItemSpawner::onCreate() {
    OSReport("Creating ItemSpawner\n");
    // Model Setup
		allocator.link(-1, GameHeaps[0], 0, 0x20);

		this->resFile.data = getResource("I_hammer", "g3d/I_fireflower.brres");
		nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("I_fireflower");
		model.setup(mdl, &allocator, 0x224, 1, 0);
		SetupTextures_Item(&model, 0);	

        OSReport("Model Setup Complete\n");

		// Animations start here
		nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
		this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);

		allocator.unlink();

        OSReport("Animation Setup Complete\n");

	// Character Setup

		scale = (Vec){1.0f, 1.0f, 1.0f};

		pos.y = pos.y - 16.0 * 3.0;
		rot.y = 0; // Y is horizontal axis
		rot.x = 0; // X is vertical axis
		rot.z = 0; // Z is ... an axis >.>

		speed.x = 0.0;
		speed.y = 0.0;
		max_speed.x = 50.0;

        OSReport("Character Setup Complete\n");


	// Physics

		HitMeBaby.xDistToCenter = 0.0;
		HitMeBaby.yDistToCenter = 8.0;

		HitMeBaby.xDistToEdge = 8.0;
		HitMeBaby.yDistToEdge = 8.0;

		HitMeBaby.category1 = 0x3;
		HitMeBaby.category2 = 0x0;
		HitMeBaby.bitfield1 = 0x47;
		HitMeBaby.bitfield2 = 0x0;

		HitMeBaby.unkShort1C = 0;
		HitMeBaby.callback = &dEn_c::collisionCallback;

		this->aPhysics.initWithStruct(this, &HitMeBaby);
		this->aPhysics.addToList();

        OSReport("Physics Setup Complete\n");

    bindAnimChr_and_setUpdateRate("wait", 0, 0.0, 1.0);

    this->onExecute();

    return true;
}

int daItemSpawner::onDelete() {
    return true;
}

int daItemSpawner::onExecute() {
    updateModelMatrices();
    return true;
}

int daItemSpawner::onDraw() {
    model.scheduleForDrawing();
	model._vf1C();
    return true;
}

void daItemSpawner::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	OSReport("Binding Animation for body: %s\n", name);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->model, anmChr, unk);
	this->model.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

extern "C" void *dAcPy_c__ChangePowerupWithAnimation(void * Player, int powerup);

void daItemSpawner::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    dAcPy_c__ChangePowerupWithAnimation(apOther->owner, 7);
    // if (this->counter_504[apOther->owner->which_player]) {
	// 		if (apThis->info.category2 == 0x9) {
	// 			dAcPy_c__ChangePowerupWithAnimation(apOther->owner, 7);
	// 		}

	// 		//else { dAcPy_vf3F4(apOther->owner, this, 9); }
	// 	}

	// this->counter_504[apOther->owner->which_player] = 0x20;
}

bool daItemSpawner::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther)
{
    this->daItemSpawner::playerCollision(apThis, apOther);
    return true;
}

void daItemSpawner::updateModelMatrices() {
    // This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y + 0.0, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	model.setDrawMatrix(matrix);
	model.setScale(&scale);
	model.calcWorld(false);

}