#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <stage.h>
#include <profile.h>
#include "boss.h"

#define hp 30
#define ROT_THRESHOLD 30
#define GRAVITY 0.0581

const char* SSarcNameListJr [] = {
	"koopaJr",
	"boss_ef_attack",
	NULL
};

class daBossBowserJrEvil : public daBoss {
public:
    int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	//nw4r::g3d::ResFile resFile2;
	m3d::mdl_c bodyModel;
	m3d::mdl_c shellModel;
	m3d::anmChr_c chrAnimation;
	m3d::anmChr_c shellAnimation;

	mEf::es2 effect;

	int isBigBoss;
	int jumpCounter;
	char isDown;
	Vec initialPos;
	Vec finalPos;
	float XSpeed;
	u32 cmgr_returnValue;
	dStageActor_c *chosenOne;
	bool topHurts;
	bool slowDown;
	bool isBouncing;
	bool walkStateIsCharging;
	float amountCharged;
	int timer2;
	bool displayShell;
	int waitTimer;
	bool playerStunned[4];
	short dir;
	int playerpos;
	int eventcount;

	int abs(int value);

    ActivePhysics Chuckles;
	ActivePhysics Knuckles;

	ActivePhysics::Info HitMeBaby;
	
	static daBossBowserJrEvil *build();

	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	bool bindAnimChrShell(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	bool calculateTileCollisions();

	void powBlockActivated(bool isNotMPGP);
	void stunPlayers();
	void unstunPlayers();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);

	void addScoreWhenHit(void *other);
	int randomPlayer();
	void followPlayer();
	void lightningStrike();

	bool isNearWall();

	USING_STATES(daBossBowserJrEvil);

	DECLARE_STATE(Intro);
	DECLARE_STATE(Outro);
	DECLARE_STATE(Idle);
	DECLARE_STATE(Fireball);
	DECLARE_STATE(Damage);
	DECLARE_STATE(Meteor);
	DECLARE_STATE(Lightning);
	DECLARE_STATE(Jump);
	DECLARE_STATE(Dragon);
	DECLARE_STATE(Bonus);

};

class daEvilFire : public dEn_c {
public:
	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;

	m3d::mdl_c model;
	mEf::es2 effect;
	m3d::anmTexSrt_c anm;

	u32 cmgr_returnValue;
	int timer;
	int angle;
	bool mode;

	ActivePhysics::Info HitMeBaby;
	
	static dActor_c *build();

	Vec directionHead;

	void updateModelMatrices();
	bool calculateTileCollisions();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);

	USING_STATES(daEvilFire);

	DECLARE_STATE(Normal);

};

class daEvilLightning : public dEn_c {
	public:
		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();

		mEf::es2 head;
		mEf::es2 bolt;

		ActivePhysics::Info HitMeBaby;
		ActivePhysics Lightning;
		u32 cmgr_returnValue;
		int timer;
		float leader;
		pointSensor_s below;
		float boltsize;
		float boltpos;
		
		static dActor_c *build();

		void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
		void lightningStrike();

		USING_STATES(daEvilLightning);

		DECLARE_STATE(Normal);
	};

    int daBossBowserJrEvil::abs(int value)
    {
        return value < 0 ? -value : value;
    }

    daBossBowserJrEvil *daBossBowserJrEvil::build()
    {
        OSReport("Building daBossBowserJrEvil\n");
        void *buffer = AllocFromGameHeap1(sizeof(daBossBowserJrEvil));
        return new (buffer) daBossBowserJrEvil;
    }

// daEvilFire *daEvilFire::build() {
// 	OSReport("Building daEvilFire\n");
// 	void *buffer = AllocFromGameHeap1(sizeof(daEvilFire));
// 	return new(buffer) daEvilFire;
// }

dActor_c *daEvilFire::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daEvilFire));
	daEvilFire *c = new(buffer) daEvilFire;

	return c;
}

dActor_c *daEvilLightning::build() {
	void *buffer = AllocFromGameHeap1(sizeof(daEvilLightning));
	daEvilLightning *c = new(buffer) daEvilLightning;

	return c;
}

const char *EvilFireFileList[] = {"boss_ef_attack", NULL };

const char *EvilLightningFileList[] = {NULL};

const SpriteData daEvilFireSpriteData = { ProfileId::EvilFire, 0, 0, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile daEvilFireProfile(&daEvilFire::build, SpriteId::EvilFire, &daEvilFireSpriteData, ProfileId::EvilFire, ProfileId::EvilFire, "EvilFire", EvilFireFileList, 0x80);

const SpriteData daEvilLightningSpriteData = { ProfileId::EvilLightning, 0, 0, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile daEvilLightningProfile(&daEvilLightning::build, SpriteId::EvilLightning, &daEvilLightningSpriteData, ProfileId::EvilLightning, ProfileId::EvilLightning, "EvilLightning", EvilLightningFileList, 0x80);

//const SpriteData daBossBowserJrEvilSpriteData = { ProfileId::daBossBowserJrEvil, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
//Profile daBossBowserJrEvilProfile(&daBossBowserJrEvil::build, SpriteId::daBossBowserJrEvil, &daBossBowserJrEvilSpriteData, ProfileId::daBossBowserJrEvil, ProfileId::daBossBowserJrEvil, "daBossBowserJrEvil", daBossBowserJrEvilFileList, 0);

///////////////////////
// Externs and States
///////////////////////
	extern "C" void *EN_LandbarrelPlayerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	// CalculateDistanceFromActorsNextPosToFurthestPlayer???
	extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);
	extern "C" void dAcPy_vf3F8(void* player, dEn_c* monster, int t);
	extern "C" void *StageScreen;


	CREATE_STATE(daBossBowserJrEvil, Intro);
	CREATE_STATE(daBossBowserJrEvil, Outro);
	CREATE_STATE(daBossBowserJrEvil, Idle);
	CREATE_STATE(daBossBowserJrEvil, Fireball);
	CREATE_STATE(daBossBowserJrEvil, Damage);
	CREATE_STATE(daBossBowserJrEvil, Meteor);
	CREATE_STATE(daBossBowserJrEvil, Lightning);
	CREATE_STATE(daBossBowserJrEvil, Jump);
	CREATE_STATE(daBossBowserJrEvil, Dragon);
	CREATE_STATE(daBossBowserJrEvil, Bonus);


	CREATE_STATE(daEvilFire, Normal);
	CREATE_STATE(daEvilLightning, Normal);

void daBossBowserJrEvil::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {

	// apOther->someFlagByte |= 2;
	// this->playEnemyDownSound1();
	// damage += 5;
	// if (damage >= hp) { doStateChange(&StateID_Outro); }
	// else { doStateChange(&StateID_Damage); }
	// deathInfo.isDead = 0;
	// this->flags_4FC |= (1<<(31-7));
	// this->counter_504[apOther->owner->which_player] = 0;
	bouncePlayerWhenJumpedOn(apOther->owner);

	char hitType = 0;
		// this is shit code
		dStateBase_c *whatState = acState.getCurrentState();
		if (whatState == &StateID_Damage | whatState == &StateID_Outro) {
			// nothing
		} else {
			hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);
		}

		if (hitType == 2) {
			// Mini jump
			apOther->someFlagByte |= 2;
		} else if (hitType > 0) {
			apOther->someFlagByte |= 2;
			bouncePlayerWhenJumpedOn(apOther->owner);

		}
		else {
			this->dEn_c::playerCollision(apThis, apOther);
			this->_vf220(apOther->owner);
		}

		deathInfo.isDead = 0;
		this->flags_4FC |= (1<<(31-7));
		this->counter_504[apOther->owner->which_player] = 0;
}

void daBossBowserJrEvil::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) { this->playerCollision(apThis, apOther); }
bool daBossBowserJrEvil::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	return collisionCat7_GroundPound(apThis, apOther);
}
bool daBossBowserJrEvil::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	apOther->someFlagByte |= 2;

	if (this->isDown == 0 & acState.getCurrentState() != &StateID_Damage & acState.getCurrentState() != &StateID_Outro) {
		damage += 5;
		if (damage >= hp) { doStateChange(&StateID_Outro); }
		else { doStateChange(&StateID_Damage); }
	}


	deathInfo.isDead = 0;
	this->flags_4FC |= (1<<(31-7));
	this->counter_504[apOther->owner->which_player] = 5;
	// bouncePlayerWhenJumpedOn(apOther->owner);
	return true;
}
bool daBossBowserJrEvil::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCat7_GroundPound(apThis, apOther);
}
bool daBossBowserJrEvil::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossBowserJrEvil::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}
bool daBossBowserJrEvil::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther){
	dAcPy_vf3F8(apOther->owner, this, 3);

	deathInfo.isDead = 0;
	this->flags_4FC |= (1<<(31-7));
	this->counter_504[apOther->owner->which_player] = 0;
	return true;
}
bool daBossBowserJrEvil::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){
	return true;
}
bool daBossBowserJrEvil::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossBowserJrEvil::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daBossBowserJrEvil::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}

void daBossBowserJrEvil::addScoreWhenHit(void *other) { }
void daBossBowserJrEvil::powBlockActivated(bool isNotMPGP) { }

bool daBossBowserJrEvil::calculateTileCollisions() {
	// Returns true if sprite should turn, false if not.

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();


	if (collMgr.isOnTopOfTile()) {
		// Walking into a tile branch

		speed.y = 0.0f;
	}

	// Bouncing checks
	if (_34A & 4) {
		Vec v = (Vec){0.0f, 1.0f, 0.0f};
		collMgr.pSpeed = &v;

		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;

		collMgr.pSpeed = &speed;

	} else {
		if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
			speed.y = 0.0f;
	}

	collMgr.calculateAdjacentCollision(0);

	// Switch Direction
	if (collMgr.outputMaybe & (0x15 << direction)) {
		pos.x += direction ? 2.0 : -2.0;
		return true;
	}
	return false;
}

void daBossBowserJrEvil::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	OSReport("Binding Animation for body: %s\n", name);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

bool daBossBowserJrEvil::bindAnimChrShell(const char* name, int unk, float unk2, float rate) {
	OSReport("Binding Animation for shell: %s\n", name);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	if (!anmChr.data || !this->shellModel.scnObj) {
		OSReport("Skipping shell bind; animation or model not ready\n");
		return false;
	}
	OSReport("Got animation, binding it\n");
	this->shellAnimation.bind(&this->shellModel, anmChr, unk);
	this->shellModel.bindAnim(&this->shellAnimation, unk2);
	this->shellAnimation.setUpdateRate(rate);
	return true;
}

void daBossBowserJrEvil::followPlayer(){
	int playerpos = GetSpecificPlayerActor(0)->pos.x-275;
	//OSReport("Player X: %d\n", playerpos);
	//this->rot.y = (float(playerpos) / 378.0) * 0x8000 - 0x4000;
	int diff = playerpos - 208 + 16;
	//OSReport("Diff: %d\n", diff);
	if (diff > ROT_THRESHOLD) {
		SmoothRotation(&this->rot.y, -0xD800, 0x800);
		dir=1;
	} else if (diff < -ROT_THRESHOLD) {
		SmoothRotation(&this->rot.y, 0xD800, 0x800);
		dir=-1;
	} else {
		SmoothRotation(&this->rot.y, 0, 0x800);
		dir=0;
	}
}

int daBossBowserJrEvil::onCreate() {
	displayShell = false;
	waitTimer = 0;

	// Model Setup
		allocator.link(-1, GameHeaps[0], 0, 0x20);

		this->resFile.data = getResource("koopaJr", "g3d/koopaJr.brres");
		nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("koopaJr");
		bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
		//SetupTextures_Enemy(&this->bodyModel, 0);
		SetupTextures_Boss(&bodyModel, 0);	

		nw4r::g3d::ResMdl shellMdl = this->resFile.GetResMdl("koopaJr_shell");
		shellModel.setup(shellMdl, &allocator, 0x224, 1, 0);
		SetupTextures_Boss(&shellModel, 0);

		// Animations start here
		nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait");
		this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);
		nw4r::g3d::ResAnmChr shellAnmChr = this->resFile.GetResAnmChr("shell_model_out");
		this->shellAnimation.setup(shellMdl, shellAnmChr, &this->allocator, 0);

		allocator.unlink();

	// Character Setup

		scale = (Vec){1.0f, 1.0f, 1.0f};
		this->timer = 0;
		this->damage = 0;
		this->jumpCounter = 0;
		this->isDown = 0;
		this->XSpeed = 0.0f;
		this->chosenOne = 0;
		this->slowDown = false;
		this->isBouncing = false;
		this->walkStateIsCharging = false;
		this->amountCharged = 0.0f;
		this->timer2 = 0;
		this->eventcount = 0;

		pos.y = pos.y - 16.0 * 3.0;
		rot.y = 0xD800; // Y is horizontal axis
		rot.x = 0; // X is vertical axis
		rot.z = 0; // Z is ... an axis >.>
		direction = 1; // Heading left.

		

		speed.x = 0.0;
		speed.y = 0.0;
		max_speed.x = 50.0;
		initialPos = pos;
		topHurts = false;

	// Physics

		HitMeBaby.xDistToCenter = 0.0;
		HitMeBaby.yDistToCenter = 11.0;

		HitMeBaby.xDistToEdge = 7.5;
		HitMeBaby.yDistToEdge = 11.0;

		HitMeBaby.category1 = 0x3;
		HitMeBaby.category2 = 0x0;
		HitMeBaby.bitfield1 = 0x4F;
		HitMeBaby.bitfield2 = 0xffbafffe;

		HitMeBaby.unkShort1C = 0;
		HitMeBaby.callback = &dEn_c::collisionCallback;

		this->aPhysics.initWithStruct(this, &HitMeBaby);
		this->aPhysics.addToList();

	// // Expulser Physics
	// 	HitMyShieldBaby.xDistToCenter = 0.0;									//Set Kamek's shield's Collision's X distance from center to 0
	// 	HitMyShieldBaby.yDistToCenter = 11;									//Set Kamek's shield's Collision's Y distance from center to 44

	// 	HitMyShieldBaby.xDistToEdge = 7.5;										//Set Kamek's shield's Collision's X scale to 22.5
	// 	HitMyShieldBaby.yDistToEdge = 11;										//Set Kamek's shield's Collision's X scale to 3

	// 	HitMyShieldBaby.category1 = 0x3;										//Ask Treeki if you want to know what that means. it's been years since i try to figure it out and i still don't know.
	// 	HitMyShieldBaby.category2 = 0x0;										//^
	// 	HitMyShieldBaby.bitfield1 = 0x4F;										//^
	// 	HitMyShieldBaby.bitfield2 = 0x8028E;									//^
	// 	HitMyShieldBaby.unkShort1C = 0;											//^
	// 	HitMyShieldBaby.callback = &dEn_c::collisionCallback;					//Set the collision callback to the dEn_c's one

	// Tile collider

		// These fucking rects do something for the tile rect
		spriteSomeRectX = 48.0f;
		spriteSomeRectY = 36.0f;
		_320 = 0.0f;
		_324 = 18.0f;

		// These structs tell stupid collider what to collide with - these are from koopa troopa
		static const lineSensor_s below(-0<<12, 0<<12, 0<<12);
		static const pointSensor_s above(0<<12, 12<<12);
		static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

		collMgr.init(this, &below, &above, &adjacent);
		collMgr.calculateBelowCollisionWithSmokeEffect();

		cmgr_returnValue = collMgr.isOnTopOfTile();

	doStateChange(&StateID_Intro);

	this->onExecute();
	return true;

}

int daBossBowserJrEvil::onDelete() {
	return true;
}

int daBossBowserJrEvil::onExecute() {
	acState.execute();
	updateModelMatrices();

	return true;
}

int daBossBowserJrEvil::onDraw() {
	if(displayShell){
		shellModel.scheduleForDrawing();
		shellModel._vf1C();
	}
	else if(acState.getCurrentState() != &StateID_Outro && acState.getCurrentState() != &StateID_Dragon) {
		followPlayer();
	}
	bodyModel.scheduleForDrawing();
	bodyModel._vf1C();
	
	return true;
}
void daBossBowserJrEvil::updateModelMatrices() {
	// This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y + 0.0, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);

	if(displayShell) {
		shellModel.setDrawMatrix(matrix);
		shellModel.setScale(&scale);
		shellModel.calcWorld(false);
	}
}

extern "C" void stunPlayer(void *, int);
extern "C" void unstunPlayer(void *);

void daBossBowserJrEvil::stunPlayers() {
	for (int i = 0; i < 4; i++) {
		playerStunned[i] = false;

		dStageActor_c *player = GetSpecificPlayerActor(i);
		if (player) {
			if (player->collMgr.isOnTopOfTile() && player->currentZoneID == currentZoneID) {
				stunPlayer(player, 1);
				playerStunned[i] = true;
			}
		}
	}
}

void daBossBowserJrEvil::unstunPlayers() {
	for (int i = 0; i < 4; i++) {
		dStageActor_c *player = GetSpecificPlayerActor(i);
		if (player && playerStunned[i]) {
			unstunPlayer(player);
		}
	}
}


///////////////
// Intro State
///////////////
void daBossBowserJrEvil::beginState_Intro() {
	this->timer = 0;
	this->timer2 = 0;
	this->bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);
	StopBGMMusic();

	dStage32C_c::instance->freezeMarioBossFlag = 1;
	WLClass::instance->_4 = 4;
	WLClass::instance->_8 = 0;

	MakeMarioEnterDemoMode();

	// pos.x = pos.x - 224.0;
	// pos.y = pos.y + 320.0;

}

void daBossBowserJrEvil::executeState_Intro() {
	if (timer == 60) {
		this->bindAnimChr_and_setUpdateRate("prov", 1, 0.0, 1.0);
		PlaySound(this, 1251);
	}
	if (timer > 60) {
		calculateTileCollisions();
	}
	if (timer == 145) {
		this->flags_4FC &= ~(1<<(31-7));
		doStateChange(&StateID_Idle);
	}
	timer++;
}

void daBossBowserJrEvil::endState_Intro() {
	MakeMarioExitDemoMode();
}

extern void *_8042A788;
extern void playFanfare(void *, int type);
// Outro State
void daBossBowserJrEvil::beginState_Outro() {
	OSReport("Begin Outro State\n");
	OutroSetup(this);
		aPhysics.removeFromList();

		this->max_speed.x = 0.0f;
		this->speed.x = 0.0f;
		this->x_speed_inc = 0.0f;

		this->max_speed.y = -2.0f;
		this->speed.y = -2.0f;
		this->y_speed_inc = -0.1875f;

		this->timer = 0;
		this->isDown = 1;

		Chuckles.removeFromList();
		Knuckles.removeFromList();
		PlaySound(this, SE_BOSS_CMN_DAMAGE_LAST);
		this->bindAnimChr_and_setUpdateRate("shell_in", 1, 0.0, 1.0);
		displayShell = this->bindAnimChrShell("shell_model_in", 1, 0.0, 1.0);
		
	updateModelMatrices();
}

void daBossBowserJrEvil::executeState_Outro() {
	calculateTileCollisions();


	if (this->dying == 1) {
		if (this->timer > 60 + 550) { 
			RESTART_CRSIN_LevelStartStruct.purpose = 0;
			RESTART_CRSIN_LevelStartStruct.world1 = 7;
			RESTART_CRSIN_LevelStartStruct.world2 = 7;
			RESTART_CRSIN_LevelStartStruct.level1 = 40;
			RESTART_CRSIN_LevelStartStruct.level2 = 40;
			RESTART_CRSIN_LevelStartStruct.areaMaybe = 0;
			RESTART_CRSIN_LevelStartStruct.entrance = 0xFF;
			RESTART_CRSIN_LevelStartStruct.unk4 = 0; // load replay
			DontShowPreGame = true;
			ExitStage(RESTART_CRSIN, 0, BEAT_LEVEL, MARIO_WIPE);
		}
		if (this->timer == 60) { 
			playFanfare(_8042A788, 4);
			PlayerVictoryCries(this); 
		}
		this->timer += 1;
		return;
	}

	if ((chrAnimation.getCurrentFrame() == 41.0) || (chrAnimation.getCurrentFrame() == 62.0)) {
		//SpawnEffect("Wm_en_landsmoke_s", 0, &(Vec){pos.x, pos.y - 8.0f, pos.z + 500.0f}, &(S16Vec){0,0,0}, &(Vec){1.0f, 1.0f, 1.0f});
	}

	if (timer == 20 && !dying) {
		//SpawnEffect("Wm_ob_cmnshotstar", 0, &(Vec){pos.x + 8.0f, pos.y - 8.0f, pos.z + 500.0f}, &(S16Vec){0,0,0}, &(Vec){1.75f, 1.75f, 1.75f});
		//SpawnEffect("Wm_mr_wirehit_hit", 0, &(Vec){pos.x + 8.0f, pos.y, pos.z + 500.0f}, &(S16Vec){0,0,0}, &(Vec){1.0f, 1.0f, 1.0f});

		PlaySound(this, STRM_BGM_SHIRO_BOSS_CLEAR);
		BossGoalForAllPlayers();

		this->dying = 1;
		this->timer = 0;
	}

	this->timer += 1;
}
void daBossBowserJrEvil::endState_Outro() {}

// Idle State
void daBossBowserJrEvil::beginState_Idle() {
	this->timer = 0;
	this->waitTimer = 25 + GenerateRandomNumber(30);
	this->bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1.0);

	
}
void daBossBowserJrEvil::executeState_Idle() {
	
	if(this->chrAnimation.isAnimationDone()) {
			this->chrAnimation.setCurrentFrame(0.0);
	}
	if(timer == waitTimer) {
		
		int random = GenerateRandomNumber((this->damage)/5);
		if(random == 0) {
			int random = GenerateRandomNumber((hp-this->damage)/3);
			OSReport("Random, is doing Bonus?: %d\n", random);
			if (random == 0 && eventcount < 2) doStateChange(&StateID_Bonus);
			else doStateChange(&StateID_Fireball);}
		else if(random == 1) doStateChange(&StateID_Jump);
		else if(random == 2) doStateChange(&StateID_Lightning);
		else if(random == 3) doStateChange(&StateID_Meteor);
		else if(random == 4) doStateChange(&StateID_Dragon);
	}

	if(timer2 == 50){
		unstunPlayers();
	}
	calculateTileCollisions();
	timer++;
	timer2++;
}
void daBossBowserJrEvil::endState_Idle() {}

// Fireball State
void daBossBowserJrEvil::beginState_Fireball() {
	this->timer = 0;

	bindAnimChr_and_setUpdateRate("stand_fire", 1, 0.0, 1.0);
}
void daBossBowserJrEvil::executeState_Fireball() {
	if(chrAnimation.getCurrentFrame() == 89) {
		u32 firesettings;
		firesettings = firesettings | (0 << 1);
		//OSReport("rot.y: %d\n", this->rot.y);
		//float turnoffset = (this->rot.y) * (10.0f / 0x8000) * 1.5f;
		float turnoffset = this->dir * 7.5f;
		OSReport("turnoffset: %f\n", turnoffset);
		Vec firepos = {this->pos.x+turnoffset, this->pos.y + 13.0f, this->pos.z + 10.0f};
		PlaySound(this, 1456);
		dStageActor_c *spawner = CreateActor(751, firesettings, firepos, 0, 0);
	}
	if(chrAnimation.isAnimationDone()) {
		doStateChange(&StateID_Idle);
	}
	timer++;
}
void daBossBowserJrEvil::endState_Fireball() {}

// Damage State
void daBossBowserJrEvil::beginState_Damage() {
	OSReport("Beginning Damage State\n");
	this->timer = 0;
	PlaySound(this, 1299);
	this->bindAnimChr_and_setUpdateRate("shell_in", 1, 0.0, 1.0);
	displayShell = this->bindAnimChrShell("shell_model_in", 1, 0.0, 1.0);
	HitMeBaby.category1 = 0x0;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x0;
	HitMeBaby.bitfield2 = 0x0;
	aPhysics.removeFromList();

}
void daBossBowserJrEvil::executeState_Damage() {
	if(timer==3) PlaySound(this, 1291);
	if(timer==40){
		HitMeBaby.category1 = 0x3;
		HitMeBaby.category2 = 0x0;
		HitMeBaby.bitfield1 = 0x6F;
		HitMeBaby.bitfield2 = 0xffbafffc; 
		aPhysics.addToList();
	}

	if(timer == 120 && displayShell) {
		PlaySound(this, 1292);
		this->bindAnimChr_and_setUpdateRate("shell_out", 1, 0.0, 1.0);
		this->bindAnimChrShell("shell_model_out", 1, 0.0, 1.0);
	}

	if(timer==140){
		doStateChange(&StateID_Idle);
		displayShell = false;
	}
	calculateTileCollisions();
	timer++;

}
void daBossBowserJrEvil::endState_Damage() {
	
	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xffbafffe;
}

// Meteor State
void daBossBowserJrEvil::beginState_Meteor() {
	this->timer = 0;
	

	bindAnimChr_and_setUpdateRate("go", 1, 0.0, 1.0);
	PlaySound(this, 1253);
}
void daBossBowserJrEvil::executeState_Meteor() {
	if(chrAnimation.isAnimationDone()) {
		playerpos = GetSpecificPlayerActor(0)->pos.x-275*0;
		OSReport("Player X: %d\n", playerpos);
		VEC3 skypos = {f32(playerpos), pos.y+f32(5.5)*16.0f, 0.0f};
		dStageActor_c *spawner = CreateActor(346, 0, skypos, 0, 0);
		doStateChange(&StateID_Idle);
	}
}
void daBossBowserJrEvil::endState_Meteor() {}

// Lightning State
void daBossBowserJrEvil::beginState_Lightning() {
	OSReport("Beginning Lightning State\n");
	this->timer = 0;
	

	bindAnimChr_and_setUpdateRate("go", 1, 0.0, 1.0);
	PlaySound(this, 1253);
}
void daBossBowserJrEvil::executeState_Lightning() {
	if(chrAnimation.isAnimationDone()) {
		OSReport("Lightning animation done\n");
		playerpos = GetSpecificPlayerActor(0)->pos.x-275*0;
		OSReport("Player X: %d\n", playerpos);
		VEC3 skypos = {f32(playerpos), pos.y+f32(5.5)*16.0f, 0.0f};
		dStageActor_c *spawner = CreateActor(752, 0, skypos, 0, 0);
		doStateChange(&StateID_Idle);
	}
	timer++;
	
}
void daBossBowserJrEvil::endState_Lightning() {}

// Jump State
void daBossBowserJrEvil::beginState_Jump() {
	this->timer = 0;
	bindAnimChr_and_setUpdateRate("jump_st", 1, 0.0, 1.0);
	PlaySound(this, 1253);
	speed.y = 3.5f;
}
void daBossBowserJrEvil::executeState_Jump() {
	speed.y += -GRAVITY;
	if(timer == 27) {
		//speed.y = 2.8f;
		//y_speed_inc = 0.15;
	}
	
	else if(timer == 28) {
		bindAnimChr_and_setUpdateRate("jumpA", 1, 0.0, 1.0);
	}

	else if(timer == 28 + 13){
		//y_speed_inc = -1;
	}

	else if(timer == 26 + 28) {
		bindAnimChr_and_setUpdateRate("jump_ed", 1, 0.0, 1.0);
	}

	if(timer == 26 + 28 + 2) {
		ShakeScreen(StageScreen, 0, 1, 0, 0);
		stunPlayers();
		PlaySound(this, SE_BOSS_MORTON_GROUND_SHAKE);
		timer2 = 0;
		
	}

	if(timer == 26 + 28 + 11) {
		doStateChange(&StateID_Idle);
	}

	calculateTileCollisions();

	timer++;
}
void daBossBowserJrEvil::endState_Jump() {}


// Dragon State

void daBossBowserJrEvil::beginState_Dragon(){
	followPlayer();
	if(!dir){
		dir = MakeRandomNumber(2);
		if(dir == 0) {
			SmoothRotation(&this->rot.y, -0xD800, 0x800);
			dir = -1;
		}
		else if(dir == 1) {
			SmoothRotation(&this->rot.y, 0xD800, 0x800);
		}

	}
	
	timer = 0;
}
void daBossBowserJrEvil::executeState_Dragon(){
	
	this->rot.y += -dir * (0xD800/4)/((20 + 22)*8);
	if(timer % 20 == 0) {
		bindAnimChr_and_setUpdateRate("stand_fire", 1, 0.0, 5.0);
	}
	else if(timer % (20 + 22) == 0) {
		u32 firesettings;
		int inv = 0;
		if(dir == -1) inv = 8;
		firesettings = firesettings | (1 << 1);
		firesettings = firesettings & ~(0xF << 2);
		firesettings = firesettings | ((abs(inv-int(timer/(20 + 22))) & 0xF )  << 2);
		//OSReport("rot.y: %d\n", this->rot.y);
		//float turnoffset = (this->rot.y) * (10.0f / 0x8000) * 1.5f;
		float turnoffset = (this->rot.y/0xD800) * 7.5f;
		OSReport("turnoffset: %f\n", turnoffset);
		Vec firepos = {this->pos.x+turnoffset, this->pos.y + 13.0f, this->pos.z + 10.0f};
		PlaySound(this, 1456);
		dStageActor_c *spawner = CreateActor(751, firesettings, firepos, 0, 0);

	}
	if(timer >= (20 + 22) * 8) {
		doStateChange(&StateID_Idle);
	}
	timer++;

}
void daBossBowserJrEvil::endState_Dragon(){}

// Bonus State

void daBossBowserJrEvil::beginState_Bonus() {
	this->timer = 0;
	this->bindAnimChr_and_setUpdateRate("prov2", 1, 0.0, 1.0);
	PlaySound(this,1252);
}

void daBossBowserJrEvil::executeState_Bonus() {
	
	if(chrAnimation.isAnimationDone()) {
		u8 event = 0x5+this->eventcount;
		dFlagMgr_c::instance->set(event, 0, true, false, false);
		doStateChange(&StateID_Idle);
		if(eventcount < 2) {
			eventcount++;
		}
	}
}

void daBossBowserJrEvil::endState_Bonus() {}

int daEvilFire::onCreate() {
	OSReport("Creating daEvilFire\n");
	mode = (settings >> 1) & 0x1;
	angle = (settings >> 2) & 0xF;
	OSReport("Angle # %d: \n", angle);

	// Model Setup 
		allocator.link(-1, GameHeaps[0], 0, 0x20);

		this->resFile.data = getResource("boss_ef_attack", "g3d/boss_ef_attack.brres");
		nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("koopa_attack");
		model.setup(mdl, &allocator, 0x224, 1, 0);
		SetupTextures_Boss(&model, 0);	

		nw4r::g3d::ResAnmTexSrt anmTexSrt = this->resFile.GetResAnmTexSrt("koopa_attack");
		this->anm.setup(mdl, anmTexSrt, &this->allocator, 0, 1);
		this->anm.bindEntry(&model,anmTexSrt, 0, 0);
		this->model.bindAnim(&this->anm, 0);
		this->anm.setFrameForEntry(0, 0.0);
		this->anm.setUpdateRateForEntry(1.0f, 0);
		this->anm.setUpdateRate(1.0);

		

		allocator.unlink();

	// Character Setup

		scale = (Vec){1.0f, 1.0f, 1.0f};
		this->timer = 0;

		rot.x = 0; // X is vertical axis
		rot.y = 0xD800; // Y is horizontal axis
		rot.z = 0; // Z is ... an axis >.>

		speed.x = 0.0;
		speed.y = 0.0;
		max_speed.x = 50.0;

	// Physics

		HitMeBaby.xDistToCenter = 0.0;
		HitMeBaby.yDistToCenter = 1.0;

		HitMeBaby.xDistToEdge = 5.5;
		HitMeBaby.yDistToEdge = 6.0;

		HitMeBaby.category1 = 0x3;
		HitMeBaby.category2 = 0x0;
		HitMeBaby.bitfield1 = 0x6F;
		HitMeBaby.bitfield2 = 0xffbafffc; 

		HitMeBaby.unkShort1C = 0;
		HitMeBaby.callback = &dEn_c::collisionCallback;

		this->aPhysics.initWithStruct(this, &HitMeBaby);
		this->aPhysics.addToList();

	// Tile collider

		// These fucking rects do something for the tile rect
		spriteSomeRectX = 48.0f;
		spriteSomeRectY = 36.0f;
		_320 = 0.0f;
		_324 = 18.0f;

		// These structs tell stupid collider what to collide with - these are from koopa troopa
		static const lineSensor_s below(-0<<12, 0<<12, 0<<12);
		static const pointSensor_s above(0<<12, 12<<12);
		static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

		collMgr.init(this, &below, &above, &adjacent);
		collMgr.calculateBelowCollisionWithSmokeEffect();

		cmgr_returnValue = collMgr.isOnTopOfTile();

		if(mode==0){
			dStageActor_c *Player = GetSpecificPlayerActor(0);

			directionHead.x = (Player->pos.x - pos.x)/sqrtf((Player->pos.x - pos.x)*(Player->pos.x - pos.x) + (Player->pos.y - pos.y)*(Player->pos.y - pos.y));
			directionHead.y = (Player->pos.y - pos.y)/sqrtf((Player->pos.x - pos.x)*(Player->pos.x - pos.x) + (Player->pos.y - pos.y)*(Player->pos.y - pos.y));

			if(directionHead.y > 0) {
				this->kill();
				this->Delete(this->deleteForever);
				// if(directionHead.x == 0) return true;
				// else {directionHead.x += directionHead.y; directionHead.y = 0; }
			} 

		}
		else if(mode==1) {
			angle = (4-angle) * 25 - 90;
			directionHead.x = (cos((angle * M_PI) / 180.0f));
			directionHead.y = (sin((angle * M_PI) / 180.0f));
		}
		speed.x = directionHead.x * 1.7;
		speed.y = directionHead.y * 1.7;

			this->rot.z = (atan2(directionHead.y, directionHead.x) - M_PI / 2.0) * (65536.0f / (2.0f * M_PI));
		OSReport("Rot Z: %f\n", this->rot.z);


	doStateChange(&StateID_Normal);
	this->onExecute();
	return true;
}

int daEvilFire::onDelete() {
	return true;
}

int daEvilFire::onExecute() {
	acState.execute();
	this->anm.process();
	updateModelMatrices();
	return true;
}

int daEvilFire::onDraw() {
	model.scheduleForDrawing();
	model._vf1C();
	return true;
}

void daEvilFire::updateModelMatrices()
{
    // This won't work with wrap because I'm lazy.
	matrix.translation(pos.x, pos.y + 8.0f, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	model.setDrawMatrix(matrix);
	model.setScale(&scale);
	model.calcWorld(false);
}

bool daEvilFire::calculateTileCollisions()
{
	// Returns true if sprite should turn, false if not.

	// HandleXSpeed();
	// HandleYSpeed();
	// doSpriteMovement();

	// this->rot.x = atan2(directionHead.y, directionHead.x) * (65536.0f / (2.0f * M_PI));
	pos.x += speed.x;
	pos.y += speed.y;

	// cmgr_returnValue = collMgr.isOnTopOfTile();
	// collMgr.calculateBelowCollisionWithSmokeEffect();


	// if (collMgr.isOnTopOfTile()) {
	// 	// Walking into a tile branch

	// 	speed.y = 0.0f;
	// }

	// // Bouncing checks
	// if (_34A & 4) {
	// 	Vec v = (Vec){0.0f, 1.0f, 0.0f};
	// 	collMgr.pSpeed = &v;

	// 	if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
	// 		speed.y = 0.0f;

	// 	collMgr.pSpeed = &speed;

	// } else {
	// 	if (collMgr.calculateAboveCollision(collMgr.outputMaybe))
	// 		speed.y = 0.0f;
	// }

	// collMgr.calculateAdjacentCollision(0);

	// // Switch Direction
	// if (collMgr.outputMaybe & (0x15 << direction)) {
	// 	pos.x += direction ? 2.0 : -2.0;
	// 	return true;
	// }
	return false;
}

void daEvilFire::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	this->dEn_c::playerCollision(apThis, apOther);
	this->_vf220(apOther->owner);
	this->flags_4FC |= (1<<(31-7));
	this->counter_504[apOther->owner->which_player] = 0;
}

bool daEvilFire::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther)
{
	this->dEn_c::playerCollision(apThis, apOther);
	return true;
}

///////////
// Normal State
////////////

void daEvilFire::beginState_Normal() {
	this->timer = 0;
}

void daEvilFire::executeState_Normal() {
	S16Vec nullRot = {0,0,0};
	Vec eff_pos = {pos.x, pos.y+5.0f, pos.z};
	this->effect.spawn("Wm_jr_fireattack", 0, &eff_pos, &nullRot, &this->scale);
	calculateTileCollisions();
}

void daEvilFire::endState_Normal() {
	
}

int daEvilLightning::onCreate() {
	OSReport("Creating daEvilLightning\n");

	// Tile collider

		// These fucking rects do something for the tile rect
		spriteSomeRectX = 48.0f;
		spriteSomeRectY = 36.0f;
		_320 = 0.0f;
		_324 = 18.0f;

		// These structs tell stupid collider what to collide with - these are from koopa troopa
		static const lineSensor_s below(-0<<12, 0<<12, 0<<12);
		static const pointSensor_s above(0<<12, 12<<12);
		static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

		collMgr.init(this, &below, &above, &adjacent);
		collMgr.calculateBelowCollisionWithSmokeEffect();

		cmgr_returnValue = collMgr.isOnTopOfTile();

	doStateChange(&StateID_Normal);
	this->onExecute();
	return true;

}

int daEvilLightning::onExecute() {
	acState.execute();
	return true;
}

int daEvilLightning::onDelete() {
	return true;
}

int daEvilLightning::onDraw() {
	return true;
}

void daEvilLightning::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	this->dEn_c::playerCollision(apThis, apOther);
	this->_vf220(apOther->owner);
	this->flags_4FC |= (1<<(31-7));
	this->counter_504[apOther->owner->which_player] = 0;
}

bool daEvilLightning::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther)
{
	this->dEn_c::playerCollision(apThis, apOther);
	return true;
}


///////////
// Normal State
////////////

void daEvilLightning::beginState_Normal() {
	this->timer = 0;

	float backupY = pos.y, backupYSpeed = speed.y;
	u32 result = 0;
	while (result == 0 && below.y > (-30 << 16)) {
		pos.y = backupY;
		below.y -= 0x4000;
		//OSReport("Sending out leader to %d", below.y>>12);

		result = collMgr.calculateBelowCollisionWithSmokeEffect();
		if (result == 0) {
			u32 tb1 = collMgr.getTileBehaviour1At(pos.x, pos.y + (below.y >> 12), 0);
			if (tb1 & 0x8000 && !(tb1 & 0x20))
				result = 1;
		}
		//OSReport("Result %d", result);
	}

	// if (result == 0) {
	// 	OSReport("Couldn't find any ground, falling back to 13 tiles distance");

	// 	leader = 13 * 16;
	// } else {
	// 	OSReport("Lightning strikes at  %d \n", below.y>>12);

		leader = -(below.y >> 12);
	// } 
	below.y = 0;
	OSReport("leader: %f\n", leader);
}

void daEvilLightning::executeState_Normal() {
	if(timer == 0) {
		PlaySound(this, SE_BOSS_JR_ELEC_APP);
	}
	if(timer > 0 && timer < 30+15) {
		S16Vec nullRot = {0,0,0};
		VEC3 efScale = {1.0f, 1.0f, 1.0f};
		head.spawn("Wm_en_birikyu", 0, &pos, &nullRot, &efScale);
		
	}
	if(timer == 30+15){
		lightningStrike();
	}

	if(timer >= 30+15 && timer < 90+15) {
		Vec efPos = {pos.x, pos.y + boltpos, pos.z};
		Vec otherEfScale = {1.0f, boltsize/36.0f, 1.0f};
		S16Vec nullRot = {0,0,0};
		bolt.spawn("Wm_jr_electricline", 0, &efPos, &nullRot, &otherEfScale);
		
	}

	if(timer == 90+15){
		this->Lightning.removeFromList();
	}
	timer++;
	
}

void daEvilLightning::endState_Normal() {
	this->Delete(1);
}

void daEvilLightning::lightningStrike() {
	OSReport("Lightning Strike!\n");
	PlaySound(this, SE_OBJ_KAZAN_ERUPTION);

	this->boltsize = (leader-14.0)/2;
	this->boltpos = -boltsize - 14.0;

	OSReport("boltsize %f, boltpos %f\n", boltsize, boltpos);

	ActivePhysics::Info Shock;
	Shock.xDistToCenter = 0;
	Shock.yDistToCenter = boltpos;

	Shock.category1 = 0x3;
	Shock.category2 = 0x9;
	Shock.bitfield1 = 0x4D;
	Shock.bitfield2 = 0x420;

	Shock.xDistToEdge = 12.0;
	Shock.yDistToEdge = boltsize;

	Shock.unkShort1C = 0;
	Shock.callback = &dEn_c::collisionCallback;

	this->Lightning.initWithStruct(this, &Shock);
	this->Lightning.addToList();

	OSReport("Spawning lightning bolt at %f, %f\n", pos.x, pos.y + boltpos);
	
}