
#include "file_scene.h"
#include "scene_graph.h"
#include "cube.h"
#include "cube_groups.h"
#include "platform.h"
#include "waypoint_types.h"
#include "player.h"
#include "trampoline.h"
#include "cannon.h"
#include "button_types.h"
#include "particle_types.h"
#include "OBJmodel.h"


FileScene::FileScene(const string& filePath) :
Scene(),
m_filePath(filePath)
{
}

bool FileScene::resetPlayer()
{
    if (m_player != NULL)
        m_player->destroy();
    
	// create the player in the scene
	m_player = new Player(this, 1.0f);
	Color glowColor = Color(0.8f, 0.9f, 0.8f, 1.0f);
	if ( !m_player->initialize(new LightTexShader(1.2f, 0.1f, 0, glowColor, false))) {
		GLOG("ERROR: Could not initialize player entity\n");
		return false;
	}
	m_player->setPosition(Vector3(10.0f, 2.0f, 0.0f));
	m_player->setYaw(180.0f);
	m_player->loadTexture("Textures/glass.tga");
	m_player->setGlowColor(glowColor);
	m_player->addToDynamicWorld(m_dynamicsWorld);
	registerForSimulationTicks(m_player);
	s_Cam()->attachTo(m_player);
    m_graph->attach(m_player);
	return true;
}


typedef enum _line_type {
    PUSH_NODE = 0,
    POP_NODE,
    END_ENTITY,
	POSITION,
	ORIENTATION,
	PLATFORM,
	CHECKPOINT,
	SH_LIGHT,
	SH_COLOR,
	SH_TEX,
	SH_GLOW,
	SH_LIGHT_TEX,
	SH_TRAMPOLINE,
	TOTAL_LINE_TYPES
} LineType;
static map<string, LineType> s_lineTypeMap;


bool FileScene::onInitialize()
{
    // register line types
    if (s_lineTypeMap.size() == 0) {
        // control lines
        s_lineTypeMap["push_node"] = PUSH_NODE;
        s_lineTypeMap["pop_node"] = POP_NODE;
        s_lineTypeMap["end_entity"] = END_ENTITY;
        
        // entities
        s_lineTypeMap["platform"] = PLATFORM;
        s_lineTypeMap["checkpoint"] = CHECKPOINT;

        // shaders
		s_lineTypeMap["sh_light"] =			SH_LIGHT;
		s_lineTypeMap["sh_color"] =			SH_COLOR;
		s_lineTypeMap["sh_tex"] =			SH_TEX;
		s_lineTypeMap["sh_glow"] =			SH_GLOW;
        s_lineTypeMap["sh_light_tex"] =		SH_LIGHT_TEX;
		s_lineTypeMap["sh_trampoline"] =	SH_TRAMPOLINE;

		// entity properties			
        s_lineTypeMap["pos"] = POSITION;
        s_lineTypeMap["ori"] = ORIENTATION;
    }
    if (s_lineTypeMap.size() != TOTAL_LINE_TYPES) {
        GLOG("ERROR: not all possible line types were registered\n");
        return false;
    }
    
    // open the file
    fstream inFile;
    inFile.open(m_filePath.c_str());
    if (!inFile.is_open()) {
        GLOG("ERROR: could not open scene file\n");
        return false;
    }
    
    // WE'LL SHARE THESE
    CollidableEntity *currentEntity = NULL; 
    OBJmodel *currentObj = NULL;

    // iterate through all lines of the file
    string line;
    while ( getline(inFile, line) ) {

		//if the line begins with #, its a comment; if it is empty continue
		if(line.empty()) continue;
		if(line.find('#') != -1) continue;

		// tested on windows side, may need #ifdef __APPLE__ here to alleviate compat issues with strings and string funcs
        // extract the type and value from the string. Type is anything before colon and value is anything after (if there is a colon. if not, whole line is type).
        string type;
        const char *after_colon;
        type = strtok((char*)line.c_str(), " \t:");
        string value;

		map<string, LineType>::iterator it;
		it = s_lineTypeMap.find(type);
		// ignore rest of line if type is one of
		if ( (*it).second != PUSH_NODE && 
				(*it).second != POP_NODE &&
				(*it).second != END_ENTITY)
		{
			// strrchr() may be an portabilty issue as the C func has prototype:
			// char* strrchr(const char*, int)  
			// here, after_colon is const
			after_colon = strrchr(line.c_str(), '\0');
			if(after_colon)
				value = string (after_colon+1);
		}

		//// extract the type and value from the string. Type is anything before colon and value is anything after (if there is a colon. if not, whole line is type).
		//string type = line.c_str();
		//char* after_colon[1];
		//type = strtok_r((char*)type.c_str(), " \t:", after_colon);
		//string value;
		//if (after_colon[0])
		//	value = string(after_colon[0]);


        // check that line has valid type
        if (s_lineTypeMap.find(type) == s_lineTypeMap.end()) {
            GLOG("WARNING: ignoring unrecognized line type in scene file: ");
            printf("'%s'\n", line.c_str());
            continue;
        }
        
        //enumerate over all types
        switch (s_lineTypeMap[type]) {
            case PUSH_NODE:
                m_graph->pushNode();
                break;
            case POP_NODE:
                m_graph->popNode();
                break;
            case END_ENTITY:
				if(currentEntity){
					//if (currentEntity == NULL) { GLOG("WARNING: scene file has end_entity without creating an entity (ignoring)\n"); continue; }
					if (!currentEntity->isInitialized()) { GLOG("WARNING: entity in scene file has not been initialize before end_entity (ignoring)\n"); continue; }
					currentEntity->addToDynamicWorld(m_dynamicsWorld);
					m_graph->attach(currentEntity);
					currentEntity = NULL;
				}else if(currentObj){ 
					if(!currentObj->isInitialized()) { GLOG("WARNING: scene file has end_entity without creating an entity (ignoring)\n"); continue; }
					currentObj->addToDynamicWorld(m_dynamicsWorld);
					m_graph->attach(currentObj);
					currentObj = NULL;
				}
                break;
			case POSITION: {
				//if (currentEntity == NULL) { GLOG("WARNING: scene file tries to set position before creating an entity (ignoring)\n"); continue; }
				float X, Y, Z;
				if (sscanf(value.c_str(), "%f, %f, %f", &X, &Y, &Z) != 3) { GLOG("ERROR reading line: "); printf("%s\n",line.c_str()); return false; }
				if(currentEntity) 
					currentEntity->setPosition(Vector3(X, Y, Z));
				else if(currentObj) 
					currentObj->setPosition(Vector3(X,Y,Z));
				break; }
			case ORIENTATION: {
				//if (currentEntity == NULL) { GLOG("WARNING: scene file tries to set orientation before creating an entity (ignoring)\n"); continue; }
				float yaw, pitch, roll;
				if (sscanf(value.c_str(), "%f, %f, %f", &yaw, &pitch, &roll) != 3) { GLOG("ERROR reading line: "); printf("%s\n",line.c_str()); return false; }
				
				if(currentEntity)
					currentEntity->setOrientation(yaw, pitch, roll);
				else if(currentObj)
					currentObj->setOrientation(yaw, pitch, roll);
				break; }

			case CHECKPOINT: {
				char f[50];	//file name-max 50 characters
				float s = 1.0f;	//scale
				//int AABBapprox, isDynamic, isFaceculled;
				//if (currentObj == NULL) { GLOG("WARNING: scene file tries to set orientation before creating an entity (ignoring)\n"); continue; }
				//int test = sscanf(value.c_str(), "%s", f);
				//int test = sscanf(value.c_str(), "%f, %s", &s, f);

				//if ( test != 2) { GLOG("ERROR reading line: "); printf("%s\n",line.c_str()); return false; }
				string g = string(f);
				g = g.substr(0, g.length());
				currentObj = new Checkpoint(this, g, s);
				break; }
            case PLATFORM: {
                float X, Y, Z;
                if (sscanf(value.c_str(), "%f, %f, %f", &X, &Y, &Z) != 3) { GLOG("ERROR reading line: "); printf("%s\n",line.c_str()); return false; }
                currentEntity = new Platform(this, Vector3(X, Y, Z));
                break; }
			case SH_COLOR:
			case SH_TEX:
			case SH_GLOW:
			case SH_LIGHT_TEX:
			case SH_TRAMPOLINE:
            case SH_LIGHT: {
                //if (currentEntity == NULL) { GLOG("WARNING: scene file tries to create shader before creating an entity (ignoring)\n"); continue; }
                float A, D; int S;
                float R, G, B, AL;
                int scanned = 0;
                if ((scanned = sscanf(value.c_str(), "%f, %f, %i, %f, %f, %f, %f", &A, &D, &S, &R, &G, &B, &AL)) < 3) {
                    GLOG("ERROR reading line: "); printf("%s\n",line.c_str()); return false; }
                Color col;
                if (scanned == 7) { col = Color(R, G, B, A); }
                else { col = RAND_COLOR; }
				if(currentEntity){
					if ( !currentEntity->initialize(new LightShader(A, D, S, col)) ) {
						GLOG("ERROR: could not initialize current entity in file scene after light shader\n"); return false; }
				}else if(currentObj){
					if ( !currentObj->initialize(new LightShader(A, D, S, col)) ) {
						GLOG("ERROR: could not initialize current objct in file scene after light shader\n"); return false; }
				}
				break; }
			
            default:
                GLOG("WARNING: unhandled LineType in FileScene\n");
                break;
        }
    }
    
	// create the player and return result
	return resetPlayer();
}


void FileScene::onUpdate(float dt)
{
}

void FileScene::onRender()
{
}

void FileScene::onShutdown()
{
}



