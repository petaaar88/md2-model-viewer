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
    ID_FRAMERATE_COMBO,
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

// Framerate options
static const f32 framerateValues[] = { 10.0f, 20.0f, 30.0f, 60.0f };
static const int FRAMERATE_COUNT = 4;

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
static IGUIComboBox* framerateCombo = nullptr;
static IGUIButton* btnBrowseWeapon = nullptr;
static IGUIButton* btnBrowseWeaponTex = nullptr;
static IGUIStaticText* lblWeaponPath = nullptr;
static IGUIStaticText* lblWeaponTex = nullptr;

// Panel dimensions
static const s32 PANEL_W = 310;

void setWeaponUIEnabled(bool enabled)
{
    btnBrowseWeapon->setEnabled(enabled);
    btnBrowseWeaponTex->setEnabled(enabled);
    editWeaponPath->setEnabled(enabled);
    editWeaponTex->setEnabled(enabled);
}

f32 getSelectedFramerate()
{
    s32 sel = framerateCombo->getSelected();
    if (sel >= 0 && sel < FRAMERATE_COUNT)
        return framerateValues[sel];
    return 10.0f;
}

void applyAnimationSpeed()
{
    f32 speed = getSelectedFramerate();
    if (playerNode)
        playerNode->setAnimationSpeed(speed);
    if (weaponNode)
        weaponNode->setAnimationSpeed(speed);
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
                    weaponNode->setMD2Animation(EMAT_STAND);
                    weaponNode->setLoopMode(true);
                }
            }
            else {
                guienv->addMessageBox(L"Warning", L"Failed to load weapon model.");
            }
        }
    }

    // Apply selected framerate
    applyAnimationSpeed();
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
                    guienv->addFileOpenDialog(L"Select MD2 Model", true, 0, ID_FILE_DIALOG_MODEL);
                    return true;
                }
                if (id == ID_BROWSE_MODEL_TEX) {
                    guienv->addFileOpenDialog(L"Select Model Texture", true, 0, ID_FILE_DIALOG_MODEL_TEX);
                    return true;
                }
                if (id == ID_BROWSE_WEAPON) {
                    guienv->addFileOpenDialog(L"Select Weapon MD2", true, 0, ID_FILE_DIALOG_WEAPON);
                    return true;
                }
                if (id == ID_BROWSE_WEAPON_TEX) {
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
                        if (weaponNode) {
                            weaponNode->setMD2Animation((EMD2_ANIMATION_TYPE)sel);
                            weaponNode->setLoopMode(true);
                        }
                    }
                    return true;
                }
                if (id == ID_FRAMERATE_COMBO) {
                    applyAnimationSpeed();
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

    // --- Load a bigger font ---
    IGUIFont* bigFont = guienv->getFont("libs/irrlicht-1.8.5/media/fonthaettenschweiler.bmp");
    if (bigFont) {
        IGUISkin* skin = guienv->getSkin();
        skin->setFont(bigFont);
    }

    // --- Build GUI panel on the left side ---
    s32 panelX = 15;
    s32 y = 15;
    s32 editW = 210;
    s32 btnW = 55;
    s32 rowH = 28;
    s32 gap = 6;

    // Background panel (a static text with border and background covering the whole panel area)
    s32 panelH = 590;
    IGUIStaticText* panelBg = guienv->addStaticText(L"",
        rect<s32>(5, 5, PANEL_W, panelH), true, true, 0, -1, true);
    panelBg->setBackgroundColor(SColor(200, 30, 30, 30));

    // Title
    IGUIStaticText* title = guienv->addStaticText(L"MD2 Model Viewer",
        rect<s32>(panelX, y, panelX + 280, y + 25), false, false, panelBg, -1, false);
    title->setOverrideColor(SColor(255, 200, 200, 255));
    y += 30;

    // Separator line (thin static text with border)
    guienv->addStaticText(L"",
        rect<s32>(panelX, y, PANEL_W - 15, y + 1), true, false, panelBg, -1, true)
        ->setBackgroundColor(SColor(255, 80, 80, 80));
    y += 8;

    // --- Model Path ---
    IGUIStaticText* lbl;
    lbl = guienv->addStaticText(L"Model (.md2):",
        rect<s32>(panelX, y, panelX + 200, y + 22), false, false, panelBg, -1, false);
    lbl->setOverrideColor(SColor(255, 220, 220, 220));
    y += 24;

    editModelPath = guienv->addEditBox(L"media/alien/tris.md2",
        rect<s32>(panelX, y, panelX + editW, y + rowH), true, panelBg, ID_MODEL_PATH_EDIT);
    guienv->addButton(rect<s32>(panelX + editW + gap, y, panelX + editW + gap + btnW, y + rowH),
        panelBg, ID_BROWSE_MODEL, L"...", L"Browse for model");
    y += rowH + gap + 4;

    // --- Model Texture ---
    lbl = guienv->addStaticText(L"Model Texture:",
        rect<s32>(panelX, y, panelX + 200, y + 22), false, false, panelBg, -1, false);
    lbl->setOverrideColor(SColor(255, 220, 220, 220));
    y += 24;

    editModelTex = guienv->addEditBox(L"media/alien/alien.pcx",
        rect<s32>(panelX, y, panelX + editW, y + rowH), true, panelBg, ID_MODEL_TEX_EDIT);
    guienv->addButton(rect<s32>(panelX + editW + gap, y, panelX + editW + gap + btnW, y + rowH),
        panelBg, ID_BROWSE_MODEL_TEX, L"...", L"Browse for texture");
    y += rowH + gap + 10;

    // Separator
    guienv->addStaticText(L"",
        rect<s32>(panelX, y, PANEL_W - 15, y + 1), true, false, panelBg, -1, true)
        ->setBackgroundColor(SColor(255, 80, 80, 80));
    y += 8;

    // --- Weapon Checkbox ---
    weaponCheckbox = guienv->addCheckBox(false,
        rect<s32>(panelX, y, panelX + 22, y + 22), panelBg, ID_WEAPON_CHECKBOX, L"");
    lbl = guienv->addStaticText(L"Load Weapon",
        rect<s32>(panelX + 24, y, panelX + 200, y + 22), false, false, panelBg, -1, false);
    lbl->setOverrideColor(SColor(255, 255, 255, 255));
    y += 28;

    // --- Weapon Path ---
    lblWeaponPath = guienv->addStaticText(L"Weapon (.md2):",
        rect<s32>(panelX, y, panelX + 200, y + 22), false, false, panelBg, -1, false);
    lblWeaponPath->setOverrideColor(SColor(255, 220, 220, 220));
    y += 24;

    editWeaponPath = guienv->addEditBox(L"media/alien/weapon.md2",
        rect<s32>(panelX, y, panelX + editW, y + rowH), true, panelBg, ID_WEAPON_PATH_EDIT);
    btnBrowseWeapon = guienv->addButton(
        rect<s32>(panelX + editW + gap, y, panelX + editW + gap + btnW, y + rowH),
        panelBg, ID_BROWSE_WEAPON, L"...", L"Browse for weapon model");
    y += rowH + gap + 4;

    // --- Weapon Texture ---
    lblWeaponTex = guienv->addStaticText(L"Weapon Texture:",
        rect<s32>(panelX, y, panelX + 200, y + 22), false, false, panelBg, -1, false);
    lblWeaponTex->setOverrideColor(SColor(255, 220, 220, 220));
    y += 24;

    editWeaponTex = guienv->addEditBox(L"media/alien/weapon.pcx",
        rect<s32>(panelX, y, panelX + editW, y + rowH), true, panelBg, ID_WEAPON_TEX_EDIT);
    btnBrowseWeaponTex = guienv->addButton(
        rect<s32>(panelX + editW + gap, y, panelX + editW + gap + btnW, y + rowH),
        panelBg, ID_BROWSE_WEAPON_TEX, L"...", L"Browse for weapon texture");
    y += rowH + gap + 10;

    // Disable weapon fields initially
    setWeaponUIEnabled(false);

    // Separator
    guienv->addStaticText(L"",
        rect<s32>(panelX, y, PANEL_W - 15, y + 1), true, false, panelBg, -1, true)
        ->setBackgroundColor(SColor(255, 80, 80, 80));
    y += 8;

    // --- Load Button ---
    guienv->addButton(rect<s32>(panelX, y, PANEL_W - 15, y + 38),
        panelBg, ID_LOAD_BUTTON, L"Load Model", L"Load the selected model and texture");
    y += 48;

    // Separator
    guienv->addStaticText(L"",
        rect<s32>(panelX, y, PANEL_W - 15, y + 1), true, false, panelBg, -1, true)
        ->setBackgroundColor(SColor(255, 80, 80, 80));
    y += 8;

    // --- Framerate Selection ---
    lbl = guienv->addStaticText(L"Animation Speed (FPS):",
        rect<s32>(panelX, y, panelX + 250, y + 22), false, false, panelBg, -1, false);
    lbl->setOverrideColor(SColor(255, 220, 220, 220));
    y += 24;

    framerateCombo = guienv->addComboBox(
        rect<s32>(panelX, y, PANEL_W - 15, y + rowH), panelBg, ID_FRAMERATE_COMBO);
    framerateCombo->addItem(L"10 FPS");
    framerateCombo->addItem(L"20 FPS");
    framerateCombo->addItem(L"30 FPS");
    framerateCombo->addItem(L"60 FPS");
    framerateCombo->setSelected(2); // default 30 FPS
    y += rowH + gap + 8;

    // --- Animation Selection ---
    lbl = guienv->addStaticText(L"Animation:",
        rect<s32>(panelX, y, panelX + 200, y + 22), false, false, panelBg, -1, false);
    lbl->setOverrideColor(SColor(255, 220, 220, 220));
    y += 24;

    animCombo = guienv->addComboBox(
        rect<s32>(panelX, y, PANEL_W - 15, y + rowH), panelBg, ID_ANIMATION_COMBO);
    for (int i = 0; i < ANIM_COUNT; i++)
        animCombo->addItem(animationNames[i]);
    animCombo->setSelected(0);

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
