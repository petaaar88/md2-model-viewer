# MD2 Model Viewer

A 3D model viewer built with the **Irrlicht Engine (v1.8.5)** for loading and previewing **MD2 (Quake II)** model files. Browse for models and textures, attach weapons, and play through all standard MD2 animations at different speeds.

## How to Use

Launch the application and a default model will load automatically from the media folder. The GUI panel on the left side contains all controls.

### Model Loading

- **Model (.md2)** - Path to the model file. Type a path manually or click `...` to browse.
- **Model Texture** - Path to the texture file. Click `...` to browse.
- Click **Load Model** to load the selected model and texture.

### Weapon (Optional)

- Check **Load Weapon** to enable weapon loading.
- **Weapon (.md2)** - Path to the weapon model file.
- **Weapon Texture** - Path to the weapon texture file.
- The weapon is attached to the player model as a child node.
- Click **Load Model** to apply changes.

### Animation Speed

Select the playback speed from the dropdown: **10**, **20**, **30**, or **60 FPS**. Default is 30 FPS. Changing this updates the animation immediately.

### Animation

Select an animation from the dropdown to play it in a loop:

| Animation | Animation | Animation | Animation |
|---|---|---|---|
| STAND | RUN | ATTACK | PAIN_A |
| PAIN_B | PAIN_C | JUMP | FLIP |
| SALUTE | FALLBACK | WAVE | POINT |
| CROUCH_STAND | CROUCH_WALK | CROUCH_ATTACK | CROUCH_PAIN |
| CROUCH_DEATH | DEATH_FALLBACK | DEATH_FALLFORWARD | DEATH_HEAD_SHOT |

### Camera Controls

| Input | Action |
|---|---|
| Left Mouse Button + Drag | Rotate the camera around the model |
| Middle Mouse Button + Drag | Pan the camera |
| Mouse Scroll Wheel | Zoom in/out |

## Media Folder

The `media/` folder contains MD2 model assets organized in subfolders. Each subfolder represents a different character or model.

**File naming conventions:**

- `tris.md2` - The main character/player model. Every model is named `tris.md2`.
- `weapon.md2` - The weapon model (not all models have one).
- `*.pcx` - Texture files. Every texture uses the `.pcx` extension. Different models may have multiple skin variations as separate `.pcx` files.

**Example structure:**

```
media/
  alien/
    tris.md2        <- player model
    alien.pcx       <- player skin
    weapon.md2      <- weapon model
    weapon.pcx      <- weapon skin
```

## Requirements

- `Irrlicht.dll` must be in the same directory as the executable
- OpenGL-compatible graphics card
- Windows operating system

## Built With

- [Irrlicht Engine 1.8.5](https://irrlicht.sourceforge.io/) - OpenGL renderer
- C++
