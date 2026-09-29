using System;
using System.Collections.Generic;
using UnityEngine;

public class PadAssignManager : MonoBehaviour
{

    private Queue<int> assignedPlayerIndices = new Queue<int>();
    private int processedPlayerCount = 0;
    private SettingUI ui;
    [SerializeField] private float sequenceDelay = 0.3f;
    float timer = 0.0f;
    [Serializable]
    public struct PlayerIcon
    {
        public GameObject ok;
        public GameObject noAssignUI;
        public GameObject assignUI;
    }

    enum eAssignState
    {
        AssignState_None,
        AssignState_Assigning,
        AssignState_AssignEnd,

    }

    private eAssignState state = eAssignState.AssignState_Assigning;

    [SerializeField] private List<PlayerIcon> playerUI;
    [SerializeField] private List<GameObject> cpuUI;
    [SerializeField] private GameObject readyUI;
    [SerializeField] private GameObject startUI;
    [SerializeField] private AudioClip assignSE;
    [SerializeField] private float pitchIncValue;
    AudioSource source;
    public void OnAssignPlayerStart()
    {
        PadAssigner.Instance.SetAssignEnabled(true);
        PadAssigner.Instance.OnPadAssigned += OnAssignPlayer;
        if (ui == null) ui = GetComponent<SettingUI>();
        source.pitch = 1-pitchIncValue;
        readyUI.SetActive(true);
        startUI.SetActive(false);
    }

    public void OnAssignPlayerEnd()
    {
        PadAssigner.Instance.SetAssignEnabled(false);
        PadAssigner.Instance.OnPadAssigned -= OnAssignPlayer;
       
        AudioSource audioSource = SettingManager.instance.GetAudioSource();
        AudioClip clip = SettingManager.instance.GetAllPadAssignedSE();
        if (audioSource != null && clip != null)
        {
            audioSource.PlayOneShot(clip);
        }
        readyUI.SetActive(false);
        startUI.SetActive(true);
        state = eAssignState.AssignState_Assigning;
        processedPlayerCount = 0;
        
    }

    public List<PlayerIcon> GetPlayerUI()
    {
        return playerUI;
    }

    public List<GameObject> GetCpuUI()
    {
        return cpuUI;
    }

    void OnAssignPlayer(int playerIndex)
    {
        assignedPlayerIndices.Enqueue(playerIndex);
    }
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        source = GetComponent<AudioSource>();
        if (ui == null) ui = GetComponent<SettingUI>();
    }

    // Update is called once per frame
    void Update()
    {

        if (ui != null && ui.GetPanelState() == SettingUI.ePanelState.PanelState_Select)
        {
            if (state == eAssignState.AssignState_Assigning)
            {
                ui.SetIsChooseEnabled(false);
                if (processedPlayerCount >= GameManager.instance.GetPlayerNum())
                {
                    readyUI.SetActive(false);
                    startUI.SetActive(true);
                    
                    state = eAssignState.AssignState_AssignEnd;
                }
               

                    while (assignedPlayerIndices.Count > 0)//パッドアサイン画面に来てここがループし出す
                    {
                        //登録するパッドがあれば、キューから取り出しアサイン
                        int playerIndex = assignedPlayerIndices.Dequeue();
                        // Debug.Log($"Player {playerIndex} assigned.");
                        if (playerIndex >= 0 && playerIndex < GameManager.instance.GetPlayerNum())
                        {
                            Debug.Log("assigned");
                            playerUI[playerIndex].assignUI.SetActive(true);
                            playerUI[playerIndex].ok.SetActive(true);
                            GameManager.instance.playerList[playerIndex].GetComponent<PlayerEntity>()
                                .RegisterPadMotor(new PlayerEntity.MotorData(0.5f, 0.5f));
                            processedPlayerCount++;

                            //アサインした音を再生し、ピッチをあげる
                            source.PlayOneShot(assignSE);
                            source.pitch += pitchIncValue;
                        }
                    }

                //パッドが接続されていなく、キーボードで操作する場合
                if (processedPlayerCount == 0 && PadAssigner.Instance.GetAssignedDeviceCount() == 0 && Input.GetKeyDown(KeyCode.Space))
                {
                    playerUI[0].assignUI.SetActive(true);
                    playerUI[0].ok.SetActive(true);
                    processedPlayerCount++;

                    //アサインした音を再生し、ピッチをあげる
                    source.PlayOneShot(assignSE);
                    source.pitch += pitchIncValue;
                }
            }
            else if (state == eAssignState.AssignState_AssignEnd)
            {
                timer += Time.deltaTime;
                if(timer>sequenceDelay)
                {
                    ui.SetIsForceEase(true);
                    timer = 0.0f;
                }
            }

        }
        else
        {
            readyUI.SetActive(false);
            startUI.SetActive(false);
        }
    }
}
