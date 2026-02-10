#define _CRT_SECURE_NO_WARNINGS
#include <irrlicht.h>
#include <iostream>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;

// GUI element IDs
enum
{
    ID_BROWSE_MODEL = 101,
    ID_BROWSE_MODEL_TEX,
    ID_BROWSE_WEAPON,
    ID_BROWSE_WEAPON_TEX,
    ID_LOAD_BUTTON,
    ID_WEAPON_CHECKBOX,
    ID_ANIMATION_COMBO,
    ID_MODEL_PATH_EDIT,
    ID_MODEL_TEX_EDIT,
    ID_WEAPON_PATH_EDIT,
    ID_WEAPON_TEX_EDIT,
    ID_FILE_DIALOG_MODEL,
    ID_FILE_DIALOG_MODEL_TEX,
    ID_FILE_DIALOG_WEAPON,
    ID_FILE_DIALOG_WEAPON_TEX
};

// Animation names matching EMAT enum order
static const wchar_t* animationNames[] = {
    L"STAND",
    L"RUN",
    L"ATTACK",
    L"PAIN_A",
    L"PAIN_B",
    L"PAIN_C",
    L"JUMP",
    L"FLIP",
    L"SALUTE",
    L"FALLBACK",
    L"WAVE",
    L"POINT",
    L"CROUCH_STAND",
    L"CROUCH_WALK",
    L"CROUCH_ATTACK",
    L"CROUCH_PAIN",
    L"CROUCH_DEATH",
    L"DEATH_FALLBACK",
    L"DEATH_FALLFORWARD",
    L"DEATH_HEAD_SHOT"
};
static const int ANIM_COUNT = 20;

// Global pointers
static IrrlichtDevice* device = nullptr;
static IVideoDriver* driver = nullptr;
static ISceneManager* smgr = nullptr;
static IGUIEnvironment* guienv = nullptr;

static IAnimatedMeshSceneNode* playerNode = nullptr;
static IAnimatedMeshSceneNode* weaponNode = nullptr;

// GUI elements
static IGUIEditBox* editModelPath = nullptr;
static IGUIEditBox* editModelTex = nullptr;
static IGUIEditBox* editWeaponPath = nullptr;
static IGUIEditBox* editWeaponTex = nullptr;
static IGUICheckBox* weaponCheckbox = nullptr;
static IGUIComboBox* animCombo = nullptr;
static IGUIButton* btnBrowseWeapon = nullptr;
static IGUIButton* btnBrowseWeaponTex = nullptr;
static IGUIStaticText* lblWeaponPath = nullptr;
static IGUIStaticText* lblWeaponTex = nullptr;

// Track which file dialog is open
static s32 activeFileDialog = 0;

void setWeaponUIEnabled(bool enabled)
{
    btnBrowseWeapon->setEnabled(enabled);
    btnBrowseWeaponTex->setEnabled(enabled);
    editWeaponPath->setEnabled(enabled);
    editWeaponTex->setEnabled(enabled);
}

void loadModel()
{
    // Remove existing nodes
    if (weaponNode) {
        weaponNode->remove();
        weaponNode = nullptr;
    }
    if (playerNode) {
        playerNode->remove();
        playerNode = nullptr;
    }

    // Load player model
    const wchar_t* modelPathW = editModelPath->getText();
    char modelPath[512];
    wcstombs(modelPath, modelPathW, 512);

    IAnimatedMesh* playerMesh = smgr->getMesh(modelPath);
    if (!playerMesh) {
        guienv->addMessageBox(L"Error", L"Failed to load player model!");
        return;
    }

    playerNode = smgr->addAnimatedMeshSceneNode(playerMesh);
    if (!playerNode) return;

    playerNode->setPosition(vector3df(0, 0, 0));
    playerNode->setMaterialFlag(EMF_LIGHTING, false);

    // Load player texture
    const wchar_t* texPathW = editModelTex->getText();
    if (wcslen(texPathW) > 0) {
        char texPath[512];
        wcstombs(texPath, texPathW, 512);
        ITexture* tex = driver->getTexture(texPath);
        if (tex)
            playerNode->setMaterialTexture(0, tex);
    }

    // Set animation from combo box
    s32 sel = animCombo->getSelected();
    if (sel >= 0 && sel < ANIM_COUNT) {
        playerNode->setMD2Animation((EMD2_ANIMATION_TYPE)sel);
    }
    else {
        playerNode->setMD2Animation(EMAT_STAND);
    }
    playerNode->setAnimationSpeed(40);
    playerNode->setLoopMode(true);

    // Load weapon if checkbox is checked
    if (weaponCheckbox->isChecked()) {
        const wchar_t* wpnPathW = editWeaponPath->getText();
        if (wcslen(wpnPathW) > 0) {
            char wpnPath[512];
            wcstombs(wpnPath, wpnPathW, 512);

            IAnimatedMesh* weaponMesh = smgr->getMesh(wpnPath);
            if (weaponMesh) {
                weaponNode = smgr->addAnimatedMeshSceneNode(weaponMesh, playerNode);
                if (weaponNode) {
                    weaponNode->setMaterialFlag(EMF_LIGHTING, false);

                    const wchar_t* wpnTexW = editWeaponTex->getText();
                    if (wcslen(wpnTexW) > 0) {
                        char wpnTexPath[512];
                        wcstombs(wpnTexPath, wpnTexW, 512);
                        ITexture* wpnTex = driver->getTexture(wpnTexPath);
                        if (wpnTex)
                            weaponNode->setMaterialTexture(0, wpnTex);
                    }
                    weaponNode->setAnimationSpeed(40);
                    weaponNode->setMD2Animation(EMAT_STAND);
                    weaponNode->setLoopMode(true);
                }
            }
            else {
                guienv->addMessageBox(L"Warning", L"Failed to load weapon model.");
            }
        }
    }
}

class MyEventReceiver : public IEventReceiver
{
public:
    virtual bool OnEvent(const SEvent& event)
    {
        if (event.EventType == EET_GUI_EVENT)
        {
            s32 id = event.GUIEvent.Caller->getID();
            EGUI_EVENT_TYPE type = event.GUIEvent.EventType;

            if (type == EGET_BUTTON_CLICKED)
            {
                if (id == ID_BROWSE_MODEL) {
                    activeFileDialog = ID_FILE_DIALOG_MODEL;
                    guienv->addFileOpenDialog(L"Select MD2 Model", true, 0, ID_FILE_DIALOG_MODEL);
                    return true;
                }
                if (id == ID_BROWSE_MODEL_TEX) {
                    activeFileDialog = ID_FILE_DIALOG_MODEL_TEX;
                    guienv->addFileOpenDialog(L"Select Model Texture", true, 0, ID_FILE_DIALOG_MODEL_TEX);
                    return true;
                }
                if (id == ID_BROWSE_WEAPON) {
                    activeFileDialog = ID_FILE_DIALOG_WEAPON;
                    guienv->addFileOpenDialog(L"Select Weapon MD2", true, 0, ID_FILE_DIALOG_WEAPON);
                    return true;
                }
                if (id == ID_BROWSE_WEAPON_TEX) {
                    activeFileDialog = ID_FILE_DIALOG_WEAPON_TEX;
                    guienv->addFileOpenDialog(L"Select Weapon Texture", true, 0, ID_FILE_DIALOG_WEAPON_TEX);
                    return true;
                }
                if (id == ID_LOAD_BUTTON) {
                    loadModel();
                    return true;
                }
            }

            if (type == EGET_FILE_SELECTED)
            {
                IGUIFileOpenDialog* dialog = (IGUIFileOpenDialog*)event.GUIEvent.Caller;
                const wchar_t* fn = dialog->getFileName();
                if (fn) {
                    s32 dlgId = dialog->getID();
                    if (dlgId == ID_FILE_DIALOG_MODEL)
                        editModelPath->setText(fn);
                    else if (dlgId == ID_FILE_DIALOG_MODEL_TEX)
                        editModelTex->setText(fn);
                    else if (dlgId == ID_FILE_DIALOG_WEAPON)
                        editWeaponPath->setText(fn);
                    else if (dlgId == ID_FILE_DIALOG_WEAPON_TEX)
                        editWeaponTex->setText(fn);
                }
                return true;
            }

            if (type == EGET_CHECKBOX_CHANGED)
            {
                if (id == ID_WEAPON_CHECKBOX) {
                    setWeaponUIEnabled(weaponCheckbox->isChecked());
                    return true;
                }
            }

            if (type == EGET_COMBO_BOX_CHANGED)
            {
                if (id == ID_ANIMATION_COMBO && playerNode) {
                    s32 sel = animCombo->getSelected();
                    if (sel >= 0 && sel < ANIM_COUNT) {
                        playerNode->setMD2Animation((EMD2_ANIMATION_TYPE)sel);
                        playerNode->setLoopMode(true);
                        // Sync weapon animation
                        if (weaponNode) {
                            weaponNode->setMD2Animation((EMD2_ANIMATION_TYPE)sel);
                            weaponNode->setLoopMode(true);
                        }
                    }
                    return true;
                }
            }
        }
        return false;
    }
};

int main()
{
    MyEventReceiver receiver;

    device = createDevice(EDT_OPENGL, dimension2d<u32>(1024, 768),
        16, false, false, false, &receiver);
    if (!device)
        return 1;

    device->setWindowCaption(L"MD2 Model Viewer");

    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();

    // --- Build GUI panel on the left side ---
    s32 panelX = 10;
    s32 y = 10;
    s32 labelW = 120;
    s32 editW = 200;
    s32 btnW = 30;
    s32 rowH = 25;
    s32 gap = 5;

    // Title
    guienv->addStaticText(L"=== MD2 Model Viewer ===",
        rect<s32>(panelX, y, panelX + 300, y + 20), false, false, 0, -1, false);
    y += rowH + gap;

    // --- Model Path ---
    guienv->addStaticText(L"Model (.md2):",
        rect<s32>(panelX, y, panelX + labelW, y + 20), false, false, 0, -1, false);
    y += 20;

    editModelPath = guienv->addEditBox(L"media/alien/tris.md2",
        rect<s32>(panelX, y, panelX + editW, y + rowH), true, 0, ID_MODEL_PATH_EDIT);
    guienv->addButton(rect<s32>(panelX + editW + gap, y, panelX + editW + gap + btnW + 20, y + rowH),
        0, ID_BROWSE_MODEL, L"...", L"Browse for model");
    y += rowH + gap + 5;

    // --- Model Texture ---
    guienv->addStaticText(L"Model Texture:",
        rect<s32>(panelX, y, panelX + labelW, y + 20), false, false, 0, -1, false);
    y += 20;

    editModelTex = guienv->addEditBox(L"media/alien/alien.pcx",
        rect<s32>(panelX, y, panelX + editW, y + rowH), true, 0, ID_MODEL_TEX_EDIT);
    guienv->addButton(rect<s32>(panelX + editW + gap, y, panelX + editW + gap + btnW + 20, y + rowH),
        0, ID_BROWSE_MODEL_TEX, L"...", L"Browse for texture");
    y += rowH + gap + 10;

    // --- Weapon Checkbox ---
    weaponCheckbox = guienv->addCheckBox(false,
        rect<s32>(panelX, y, panelX + 200, y + 20), 0, ID_WEAPON_CHECKBOX, L"Load Weapon");
    y += rowH + gap;

    // --- Weapon Path ---
    lblWeaponPath = guienv->addStaticText(L"Weapon (.md2):",
        rect<s32>(panelX, y, panelX + labelW, y + 20), false, false, 0, -1, false);
    y += 20;

    editWeaponPath = guienv->addEditBox(L"media/alien/weapon.md2",
        rect<s32>(panelX, y, panelX + editW, y + rowH), true, 0, ID_WEAPON_PATH_EDIT);
    btnBrowseWeapon = guienv->addButton(
        rect<s32>(panelX + editW + gap, y, panelX + editW + gap + btnW + 20, y + rowH),
        0, ID_BROWSE_WEAPON, L"...", L"Browse for weapon model");
    y += rowH + gap + 5;

    // --- Weapon Texture ---
    lblWeaponTex = guienv->addStaticText(L"Weapon Texture:",
        rect<s32>(panelX, y, panelX + labelW, y + 20), false, false, 0, -1, false);
    y += 20;

    editWeaponTex = guienv->addEditBox(L"media/alien/weapon.pcx",
        rect<s32>(panelX, y, panelX + editW, y + rowH), true, 0, ID_WEAPON_TEX_EDIT);
    btnBrowseWeaponTex = guienv->addButton(
        rect<s32>(panelX + editW + gap, y, panelX + editW + gap + btnW + 20, y + rowH),
        0, ID_BROWSE_WEAPON_TEX, L"...", L"Browse for weapon texture");
    y += rowH + gap + 10;

    // Disable weapon fields initially (checkbox unchecked)
    setWeaponUIEnabled(false);

    // --- Load Button ---
    guienv->addButton(rect<s32>(panelX, y, panelX + 260, y + 35),
        0, ID_LOAD_BUTTON, L"Load Model", L"Load the selected model and texture");
    y += 45;

    // --- Animation Selection ---
    guienv->addStaticText(L"Animation:",
        rect<s32>(panelX, y, panelX + labelW, y + 20), false, false, 0, -1, false);
    y += 20;

    animCombo = guienv->addComboBox(
        rect<s32>(panelX, y, panelX + 260, y + rowH), 0, ID_ANIMATION_COMBO);
    for (int i = 0; i < ANIM_COUNT; i++)
        animCombo->addItem(animationNames[i]);
    animCombo->setSelected(0); // STAND

    // --- Maya-style camera ---
    ICameraSceneNode* cam = smgr->addCameraSceneNodeMaya(
        0, -100.0f, 100.0f, 100.0f);
    cam->setTarget(vector3df(0, 10, 0));
    cam->setPosition(vector3df(0, 20, -50));
    cam->setFarValue(5000.0f);

    // Load default model on startup
    loadModel();

    while (device->run())
    {
        driver->beginScene(true, true, SColor(255, 50, 50, 50));
        smgr->drawAll();
        guienv->drawAll();
        driver->endScene();
    }

    device->drop();
    return 0;
}
