using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Controls;
using UnityEngine.InputSystem.Users;

[DefaultExecutionOrder(1000)]
public class PadAssigner : MonoBehaviour
{
    public static PadAssigner Instance { get; private set; }

    [SerializeField] private const string SchemeGamePad = "Gamepad";
    [SerializeField] private const string SchemeHybrid = "Hybrid";

    [SerializeField] private List<PlayerInput> players = new();

    private List<Action> nextFrameActions = new List<Action>();

    private int assignedDeviceCount = 0;
    [SerializeField] private Dictionary<InputDevice, int> assignedDevices = new Dictionary<InputDevice, int>();

    public event Action<int> OnPadAssigned;

    [SerializeField, Header("※デバッグ用 プレイヤー数")]
    private int maxPlayers = 4;

    public int MaxPlayers { get { return maxPlayers; } }

    private bool assignEnabled = true;

    void Awake()
    {
        if (Instance != null && Instance != this)
        {
            Destroy(this.gameObject);
            return;
        }
        Instance = this;
        if (transform.parent != null)
        {
            transform.parent = null;
        }
        DontDestroyOnLoad(this.gameObject);
        OECULogging.SetAutoCatchErrors(false);

        Gamepad[] allConnectedPads = Gamepad.all.ToArray();
        Debug.Log($"Currently connected gamepads: {string.Join(", ", allConnectedPads.Select(p => p.displayName))}");

        Joystick[] allConnectedJoysticks = Joystick.all.ToArray();
        Debug.Log($"Currently connected joysticks: {string.Join(", ", allConnectedJoysticks.Select(j => j.displayName))}");

        ButtonControl[] allJoystickButtons = allConnectedJoysticks.SelectMany(j => j.allControls.OfType<ButtonControl>()).ToArray();
        Debug.Log($"All joystick buttons: {string.Join(", ", allJoystickButtons.Select(b => $"{b.device.displayName}:{b.name}"))}");
    }

    // Start is called before the first frame update
    void Start()
    {
        assignedDeviceCount = 0;
        assignedDevices = new Dictionary<InputDevice, int>();
        InputSystem.onDeviceChange += OnDeviceChange;
    }

    void Update()
    {
        if (nextFrameActions.Count > 0)
        {
            var copiedActions = new List<Action>(nextFrameActions);
            nextFrameActions.Clear();
            foreach (var action in copiedActions)
            {
                action.Invoke();
            }
        }

        if (assignedDeviceCount < MaxPlayers && assignEnabled)
        {
            foreach (Gamepad pad in Gamepad.all)
            {
                // Debug.Log($"Checking {pad.displayName} for assignment");
                if (assignedDevices.ContainsKey(pad)) continue;

                if (pad.buttonEast.wasPressedThisFrame)
                {
                    Debug.Log($"Detected {pad.displayName} A button press for assignment");
                    Debug.Log($"Assigned {pad.displayName} to player {assignedDeviceCount}");
                    assignedDevices[pad] = assignedDeviceCount;
                    int assignedThisTime = assignedDeviceCount;
                    assignedDeviceCount++;
                    if (players.Count != 0)
                    {
                        PairPlayers();
                    }
                    OnPadAssigned?.Invoke(assignedThisTime);
                }
            }
            foreach (Joystick stick in Joystick.all)
            {
                if (assignedDevices.ContainsKey(stick)) continue;

                if (stick.GetChildControl<ButtonControl>("button2")?.wasPressedThisFrame == true)
                {
                    Debug.Log($"Detected {stick.displayName} button2 press for assignment");
                    Debug.Log($"Assigned {stick.displayName} to player {assignedDeviceCount}");
                    assignedDevices[stick] = assignedDeviceCount;
                    int assignedThisTime = assignedDeviceCount;
                    assignedDeviceCount++;
                    if (players.Count != 0)
                    {
                        PairPlayers();
                    }
                    OnPadAssigned?.Invoke(assignedThisTime);
                }
            }
        }

    }

    public PlayerInput GetPlayerInputByIndex(int index)
    {
        if (index >= 0 && index < players.Count)
            return players[index];
        else return null;
    }

    public void ApplyPlayers(List<GameObject> playerObjects)
    {
        players.Clear();
        for (int i = 0; i < playerObjects.Count; i++)
        {
            var playerInput = playerObjects[i].GetComponent<PlayerInput>();
            if (playerInput != null)
            {
                players.Add(playerInput);
            }
        }

        PairPlayers();
    }

    private void PairPlayers()
    {
        foreach (var player in players)
        {
            player.neverAutoSwitchControlSchemes = true;
            if (player.user.valid)
            {
                player.user.UnpairDevices();
            }
        }

        for (int i = 0; i < assignedDeviceCount; i++)
        {
            InputDevice device = assignedDevices.FirstOrDefault(x => x.Value == i).Key;
            Debug.Log($"Pairing player {i} with device {device?.displayName ?? "no device"}");
            if (i == 0)//キーボードとの両立のためインデックス０はこの関数を使用
            {
                PairHybrid(players[i], device);
            }
            else
            {
                PairControllerOnly(players[i], device);
            }
        }

        if (assignedDeviceCount == 0)
        {
            PairHybrid(players[0], null);
        }
    }

    void OnDestroy()
    {
        if (Instance != this) return;
        InputSystem.onDeviceChange -= OnDeviceChange;
        Instance = null;
    }

    void AssignAll()
    {
        var pads = Gamepad.all.ToList();
        Debug.Log($"Assigning pads: {string.Join(", ", pads.Select(p => p.displayName))}");

        if (players.Count > 0 && players[0] != null)
        {
            PairHybrid(players[0], pads.ElementAtOrDefault(0));
        }

        PairControllerOnly(players[1], pads.ElementAtOrDefault(1));
        PairControllerOnly(players[2], pads.ElementAtOrDefault(2));
        PairControllerOnly(players[3], pads.ElementAtOrDefault(3));
    }

    //プレイヤー側のPlayerInputとゲームパッドをペアリング
    void PairHybrid(PlayerInput pi, InputDevice device)
    {
        if (pi == null) return;
        if (!pi.user.valid)
        {
            nextFrameActions.Add(() =>
            {
                PairHybrid(pi, device);
            });
            return;
        }
        pi.user.UnpairDevices();

        if (device != null)
        {
            InputUser.PerformPairingWithDevice(device, pi.user);
            if (pi.GetComponent<Player>() != null)
            {
                pi.GetComponent<PlayerEntity>().controllerDevice = device;
                pi.GetComponent<PlayerEntity>().SetPadAssigner(this);
            }
        }
        if (Keyboard.current != null) InputUser.PerformPairingWithDevice(Keyboard.current, pi.user);
        if (Mouse.current != null) InputUser.PerformPairingWithDevice(Mouse.current, pi.user);

        var devices = new List<InputDevice>();
        if (device != null) devices.Add(device);
        if (Keyboard.current != null) devices.Add(Keyboard.current);
        if (Mouse.current != null) devices.Add(Mouse.current);

        pi.SwitchCurrentControlScheme(SchemeHybrid, devices.ToArray());
        Debug.Log($"Paired {pi.name} with {device?.displayName ?? "no device"} and hybrid scheme");
    }

    void PairControllerOnly(PlayerInput pi, InputDevice device)
    {
        if (pi == null) return;
        if (!pi.user.valid)
        {
            nextFrameActions.Add(() =>
            {
                PairControllerOnly(pi, device);
            });
            Debug.Log($"Delaying pairing of {pi.name} with {device?.displayName ?? "no device"}");
            return;
        }
        pi.user.UnpairDevices();

        if (device != null)
        {
            InputUser.PerformPairingWithDevice(device, pi.user);
            pi.SwitchCurrentControlScheme(SchemeGamePad, device);
            if (pi.GetComponent<Player>() != null)
            {
                pi.GetComponent<PlayerEntity>().controllerDevice = device;
                pi.GetComponent<PlayerEntity>().SetPadAssigner(this);
            }
        }
        Debug.Log($"Paired {pi.name} with {device?.displayName ?? "no device"}");
    }

    public void RePairDevice(PlayerInput pi, InputDevice pad)
    {
        PairControllerOnly(pi, pad);
    }

    private void OnDeviceChange(InputDevice device, InputDeviceChange change)
    {
        switch (change)
        {
            case InputDeviceChange.Added:
            case InputDeviceChange.Removed:
            case InputDeviceChange.Disconnected:
            case InputDeviceChange.Reconnected:
                if (device == null) return;
                if (device is Gamepad || device is Joystick)
                {
                    if (assignedDevices.ContainsKey(device))
                    {
                        int playerIndex = assignedDevices[device];
                        if (playerIndex >= players.Count)
                        {
                            break;
                        }
                        if (playerIndex == 0)
                        {
                            PairHybrid(players[playerIndex], device);
                        }
                        else
                        {
                            PairControllerOnly(players[playerIndex], device);
                        }
                    }
                }
                break;
        }
    }

    public void ResetAssignments()
    {
        assignedDeviceCount = 0;
        assignedDevices.Clear();
        players.Clear();
        nextFrameActions.Clear();
    }

    public int GetAssignedDeviceCount()
    {
        return assignedDeviceCount;
    }

    public void SetAssignEnabled(bool enabled)
    {
        assignEnabled = enabled;
    }
}
