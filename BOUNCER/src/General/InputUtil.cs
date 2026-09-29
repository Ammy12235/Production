using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Controls;

// InputSystemがOrder-100のため、そのすぐ後に実行されるようにする
[DefaultExecutionOrder(-99)]
public class InputUtil : MonoBehaviour
{
    public static InputUtil Instance { get; private set; }
    
    [SerializeField]
    private float joystickThreshold = 0.5f;
    
    private void Awake() {
        if (Instance != null && Instance != this) {
            DestroyImmediate(this.gameObject);
        }
        else {
            Instance = this;
            if (transform.parent != null) {
                transform.parent = null;
            }
            DontDestroyOnLoad(this.gameObject);
        }
    }
    
    #region Private Variables
    private struct InputData {
        public float Horizontal;
        public float Vertical;
        public bool East;
        public bool South;
        public bool Select;
        public int LastHorizontalDir;
        public int LastVerticalDir;
    }

    private Keyboard _keyboard;
    private List<Gamepad> _padMap;
    private List<Joystick> _joysticks;
    
    private InputData _keyboardData;
    private readonly Dictionary<Gamepad, InputData> _padData = new();
    private readonly Dictionary<Joystick, InputData> _joystickData = new();

    # region JoyStick Buttons
    private Dictionary<Joystick, ButtonControl> _joystickEnterButtons = new();
    # endregion
    
    # region Input State Trackers - joyStick
    private bool _dirChangedToUpThisFrameKeyboard;
    private readonly HashSet<Gamepad> _dirChangedToUpThisFramePads = new();
    private readonly HashSet<Joystick> _dirChangedToUpThisFrameJoysticks = new();
    
    private bool _dirChangedToDownThisFrameKeyboard;
    private readonly HashSet<Gamepad> _dirChangedToDownThisFramePads = new();
    private readonly HashSet<Joystick> _dirChangedToDownThisFrameJoysticks = new();

    private bool _dirChangedToLeftThisFrameKeyboard;
    private readonly HashSet<Gamepad> _dirChangedToLeftThisFramePads = new();
    private readonly HashSet<Joystick> _dirChangedToLeftThisFrameJoysticks = new();

    private bool _dirChangedToRightThisFrameKeyboard;
    private readonly HashSet<Gamepad> _dirChangedToRightThisFramePads = new();
    private readonly HashSet<Joystick> _dirChangedToRightThisFrameJoysticks = new();
    # endregion
    
    # region Input State Trackers - Buttons
    private bool _eastPressedThisFrameKeyboard;
    private readonly HashSet<Gamepad> _eastPressedThisFramePads = new();
    private readonly HashSet<Joystick> _enterPressedThisFrameJoysticks = new();

    private bool _southPressedThisFrameKeyboard;
    private readonly HashSet<Gamepad> _southPressedThisFramePads = new();
    
    private readonly HashSet<Gamepad> _selectPressedThisFramePads = new();
    #endregion
    #endregion
    
    #region Public Methods
    
        public bool IsAnyGamepadConnected() {
            return _padMap.Count > 0;
        }
    
        public bool IsAnyJoystickConnected() {
            return _joysticks.Count > 0;
        }

        public bool IsAnyControllerDeviceConnected() {
            return IsAnyGamepadConnected() || IsAnyJoystickConnected();
        }
    
        public bool IsAnyEastPressed() {
            if (_eastPressedThisFrameKeyboard) return true;
            if (_eastPressedThisFramePads.Count > 0) return true;
            if (_enterPressedThisFrameJoysticks.Count > 0) return true;
            return false;
        }
        
        public bool IsAnySouthPressed() {
            if (_southPressedThisFrameKeyboard) return true;
            if (_southPressedThisFramePads.Count > 0) return true;
            return false;
        }
        
        public bool IsAnySelectPressed() {
            if (_selectPressedThisFramePads.Count > 0) return true;
            return false;
        }
        
        public bool IsAnyAxisChangedToUp() {
            if (_dirChangedToUpThisFrameKeyboard) return true;
            if (_dirChangedToUpThisFramePads.Count > 0) return true;
            if (_dirChangedToUpThisFrameJoysticks.Count > 0) return true;
            return false;
        }
        
        public bool IsAnyAxisChangedToDown() {
            if (_dirChangedToDownThisFrameKeyboard) return true;
            if (_dirChangedToDownThisFramePads.Count > 0) return true;
            if (_dirChangedToDownThisFrameJoysticks.Count > 0) return true;
            return false;
        }
        
        public bool IsAnyAxisChangedToLeft() {
            if (_dirChangedToLeftThisFrameKeyboard) return true;
            if (_dirChangedToLeftThisFramePads.Count > 0) return true;
            if (_dirChangedToLeftThisFrameJoysticks.Count > 0) return true;
            return false;
        }
        
        public bool IsAnyAxisChangedToRight() {
            if (_dirChangedToRightThisFrameKeyboard) return true;
            if (_dirChangedToRightThisFramePads.Count > 0) return true;
            if (_dirChangedToRightThisFrameJoysticks.Count > 0) return true;
            return false;
        }

        public bool IsAnyAxisChanged() {
            if (IsAnyAxisChangedToUp()) return true;
            if (IsAnyAxisChangedToDown()) return true;
            if (IsAnyAxisChangedToLeft()) return true;
            if (IsAnyAxisChangedToRight()) return true;
            return false;
        }

    #endregion
    
    void Start()
    {
        _keyboard = Keyboard.current;
        _padMap = Gamepad.all.ToList();
        _joysticks = Joystick.all.ToList();

        _keyboardData = new InputData();
        foreach (var pad in _padMap) {
            _padData[pad] = new InputData();
        }
        foreach (var joystick in _joysticks) {
            _joystickData[joystick] = new InputData();
            _joystickEnterButtons[joystick] = joystick.GetChildControl<ButtonControl>("button2");
        }
        
        InputSystem.onDeviceChange += OnPadEdited;
    }
    
    private void OnPadEdited(InputDevice device, InputDeviceChange change) {
        if (device is Gamepad pad) {
            if (change == InputDeviceChange.Added) {
                _padMap.Add(pad);
                _padData[pad] = new InputData();
            }
            else if (change == InputDeviceChange.Removed || change == InputDeviceChange.Disconnected) {
                _padMap.Remove(pad);
                _padData.Remove(pad);
            } else if (change == InputDeviceChange.Reconnected) {
                if (!_padMap.Contains(pad)) {
                    _padMap.Add(pad);
                }
                _padData[pad] = new InputData();
            }
        } else if (device is Joystick joystick) {
            if (change == InputDeviceChange.Added) {
                _joysticks.Add(joystick);
                _joystickData[joystick] = new InputData();
                _joystickEnterButtons[joystick] = joystick.GetChildControl<ButtonControl>("button2");
            }
            else if (change == InputDeviceChange.Removed || change == InputDeviceChange.Disconnected) {
                _joysticks.Remove(joystick);
                _joystickData.Remove(joystick);
                _joystickEnterButtons.Remove(joystick);
            } else if (change == InputDeviceChange.Reconnected) {
                if (!_joysticks.Contains(joystick)) {
                    _joysticks.Add(joystick);
                }
                _joystickEnterButtons[joystick] = joystick.GetChildControl<ButtonControl>("button2");
                _joystickData[joystick] = new InputData();
            }
        }
    }

    void Update() {
        UpdateKeyboardData();
        foreach (var pad in _padMap) {
            UpdatePadData(pad);
        }
        foreach (var joystick in _joysticks) {
            UpdateJoystickData(joystick);
        }
    }
    
    private void UpdateKeyboardData() {
        // Check Tracking Variables (ignore Select for keyboard)
        float horizontal = 0f;
        float vertical = 0f;
        
        bool east = _keyboard.spaceKey.isPressed;
        bool south = _keyboard.escapeKey.isPressed;
        
        int dirHorizontal = 0;
        int dirVertical = 0;
        
        if (_keyboard.leftArrowKey.isPressed || _keyboard.aKey.isPressed) {
            horizontal -= 1f;
        }
        if (_keyboard.rightArrowKey.isPressed || _keyboard.dKey.isPressed) {
            horizontal += 1f;
        }
        
        if (_keyboard.upArrowKey.isPressed || _keyboard.wKey.isPressed) {
            vertical += 1f;
        }
        if (_keyboard.downArrowKey.isPressed || _keyboard.sKey.isPressed) {
            vertical -= 1f;
        }

        if (horizontal < -joystickThreshold) {
            dirHorizontal = -1;
        } else if (horizontal > joystickThreshold) {
            dirHorizontal = 1;
        }
        
        if (vertical < -joystickThreshold) {
            dirVertical = -1;
        } else if (vertical > joystickThreshold) {
            dirVertical = 1;
        }
        
        // Update state trackers
        if (dirVertical == 1 && _keyboardData.LastVerticalDir != 1) {
            _dirChangedToUpThisFrameKeyboard = true;
        } else {
            _dirChangedToUpThisFrameKeyboard = false;
        }
        if (dirVertical == -1 && _keyboardData.LastVerticalDir != -1) {
            _dirChangedToDownThisFrameKeyboard = true;
        } else {
            _dirChangedToDownThisFrameKeyboard = false;
        }
        
        if (dirHorizontal == -1 && _keyboardData.LastHorizontalDir != -1) {
            _dirChangedToLeftThisFrameKeyboard = true;
        } else {
            _dirChangedToLeftThisFrameKeyboard = false;
        }
        if (dirHorizontal == 1 && _keyboardData.LastHorizontalDir != 1) {
            _dirChangedToRightThisFrameKeyboard = true;
        } else {
            _dirChangedToRightThisFrameKeyboard = false;
        }
        
        if (east && !_keyboardData.East) {
            _eastPressedThisFrameKeyboard = true;
        } else {
            _eastPressedThisFrameKeyboard = false;
        }
        
        if (south && !_keyboardData.South) {
            _southPressedThisFrameKeyboard = true;
        } else {
            _southPressedThisFrameKeyboard = false;
        }
        
        // Update stored data
        _keyboardData.Horizontal = horizontal;
        _keyboardData.Vertical = vertical;
        _keyboardData.East = east;
        _keyboardData.South = south;
        _keyboardData.LastHorizontalDir = dirHorizontal;
        _keyboardData.LastVerticalDir = dirVertical;
    }

    private void UpdatePadData(Gamepad pad) {
        if (pad == null) return;
        InputData data = _padData[pad];
        
        // Check Tracking Variables
        float horizontal = pad.leftStick.x.ReadValue();
        float vertical = pad.leftStick.y.ReadValue();
        bool east = pad.buttonEast.isPressed;
        bool south = pad.buttonSouth.isPressed;
        bool select = pad.selectButton.isPressed;
        int dirHorizontal = 0;
        int dirVertical = 0;
        
        if (horizontal < -joystickThreshold) {
            dirHorizontal = -1;
        } else if (horizontal > joystickThreshold) {
            dirHorizontal = 1;
        }
        
        if (vertical < -joystickThreshold) {
            dirVertical = -1;
        } else if (vertical > joystickThreshold) {
            dirVertical = 1;
        }
        
        // Update state trackers
        if (dirVertical == 1 && data.LastVerticalDir != 1) {
            _dirChangedToUpThisFramePads.Add(pad);
        } else if (_dirChangedToUpThisFramePads.Contains(pad)) {
            _dirChangedToUpThisFramePads.Remove(pad);
        }
        if (dirVertical == -1 && data.LastVerticalDir != -1) {
            _dirChangedToDownThisFramePads.Add(pad);
        } else if (_dirChangedToDownThisFramePads.Contains(pad)) {
            _dirChangedToDownThisFramePads.Remove(pad);
        }
        
        if (dirHorizontal == -1 && data.LastHorizontalDir != -1) {
            _dirChangedToLeftThisFramePads.Add(pad);
        } else if (_dirChangedToLeftThisFramePads.Contains(pad)) {
            _dirChangedToLeftThisFramePads.Remove(pad);
        }
        if (dirHorizontal == 1 && data.LastHorizontalDir != 1) {
            _dirChangedToRightThisFramePads.Add(pad);
        } else if (_dirChangedToRightThisFramePads.Contains(pad)) {
            _dirChangedToRightThisFramePads.Remove(pad);
        }
        
        if (east && !data.East) {
            _eastPressedThisFramePads.Add(pad);
        } else if (_eastPressedThisFramePads.Contains(pad)) {
            _eastPressedThisFramePads.Remove(pad);
        }
        
        if (south && !data.South) {
            _southPressedThisFramePads.Add(pad);
        } else if (_southPressedThisFramePads.Contains(pad)) {
            _southPressedThisFramePads.Remove(pad);
        }
        
        if (select && !data.Select) {
            _selectPressedThisFramePads.Add(pad);
        } else if (_selectPressedThisFramePads.Contains(pad)) {
            _selectPressedThisFramePads.Remove(pad);
        }
        
        // Update stored data
        data.Horizontal = horizontal;
        data.Vertical = vertical;
        data.East = east;
        data.South = south;
        data.Select = select;
        data.LastHorizontalDir = dirHorizontal;
        data.LastVerticalDir = dirVertical;
        _padData[pad] = data;
    }

    private void UpdateJoystickData(Joystick joystick) {
        if (joystick == null) return;
        InputData data = _joystickData[joystick];
        
        // Check Tracking Variables
        float horizontal = joystick.stick.x.ReadValue();
        float vertical = joystick.stick.y.ReadValue();
        bool enter = _joystickEnterButtons[joystick].isPressed;
        int dirHorizontal = 0;
        int dirVertical = 0;
        
        if (horizontal < -joystickThreshold) {
            dirHorizontal = -1;
        } else if (horizontal > joystickThreshold) {
            dirHorizontal = 1;
        }
        
        if (vertical < -joystickThreshold) {
            dirVertical = -1;
        } else if (vertical > joystickThreshold) {
            dirVertical = 1;
        }
        
        // Update state trackers
        if (dirVertical == 1 && data.LastVerticalDir != 1) {
            _dirChangedToUpThisFrameJoysticks.Add(joystick);
        } else if (_dirChangedToUpThisFrameJoysticks.Contains(joystick)) {
            _dirChangedToUpThisFrameJoysticks.Remove(joystick);
        }
        if (dirVertical == -1 && data.LastVerticalDir != -1) {
            _dirChangedToDownThisFrameJoysticks.Add(joystick);
        } else if (_dirChangedToDownThisFrameJoysticks.Contains(joystick)) {
            _dirChangedToDownThisFrameJoysticks.Remove(joystick);
        }
        
        if (dirHorizontal == -1 && data.LastHorizontalDir != -1) {
            _dirChangedToLeftThisFrameJoysticks.Add(joystick);
        } else if (_dirChangedToLeftThisFrameJoysticks.Contains(joystick)) {
            _dirChangedToLeftThisFrameJoysticks.Remove(joystick);
        }
        if (dirHorizontal == 1 && data.LastHorizontalDir != 1) {
            _dirChangedToRightThisFrameJoysticks.Add(joystick);
        } else if (_dirChangedToRightThisFrameJoysticks.Contains(joystick)) {
            _dirChangedToRightThisFrameJoysticks.Remove(joystick);
        }
        
        if (enter && !data.East) {
            _enterPressedThisFrameJoysticks.Add(joystick);
        } else if (_enterPressedThisFrameJoysticks.Contains(joystick)) {
            _enterPressedThisFrameJoysticks.Remove(joystick);
        }

        // Update stored data
        data.Horizontal = horizontal;
        data.Vertical = vertical;
        data.East = enter;
        data.LastHorizontalDir = dirHorizontal;
        data.LastVerticalDir = dirVertical;
        _joystickData[joystick] = data;
    }

    private void OnDestroy() {
        if (Instance != this) return;
        InputSystem.onDeviceChange -= OnPadEdited;
        Instance = null;
    }
}