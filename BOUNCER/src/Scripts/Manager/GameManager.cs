
using System.Collections.Generic;
using TMPro;
using Unity.Cinemachine;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using UnityEngine.Video;

public class GameManager : MonoBehaviour
{
    public static GameManager instance;

    [SerializeField] private List<GameObject> playerGUI;
    [SerializeField] private List<Material> playerMats;
    [SerializeField] private SkinnedMeshRenderer resultModelRenderer;
    [SerializeField] private CostumeSetter resultCostumeSetter;
    private int[] memberCostumeIndices = new int[4];

    [SerializeField] private RawImage readyText;
    [SerializeField] private RawImage goText;
    [SerializeField] private Image suddenDeathText;
    [SerializeField] private GameObject finish;
    [SerializeField] private GameObject winnerText;
    [SerializeField] private Image drawText;
    [SerializeField] private Text timerText;
    [SerializeField] private GameObject playerScoreBoard;
    [SerializeField] private GameObject timerObj;
    [SerializeField] private GameObject resultTimeline;
    [SerializeField] private GameObject entities;
    [SerializeField] private GameObject walls;
    // === スコア表示 ===
    // 1) 旧：テキストベース（残っていてもOK）…自動更新 & null安全
    [SerializeField] private List<TMP_Text> scoreTexts = new List<TMP_Text>();
    // 2) 新：数字スプライト（ImageNo）…自動更新 & null安全
    [SerializeField] private List<ImageNo> scoreNumbers = new List<ImageNo>();
    // 3) playerGUI から自動バインドするか（true 推奨）
    [SerializeField] private bool autoBindScoreFromPlayerGUI = true;
    private int[] lastScores; // 差分更新用キャッシュ

    // === 追加：結果パネル / 実行フラグ ===
    [SerializeField] private GameObject resultPanel; // リザルト用パネル（Inspectorで割り当て）
    [SerializeField] private GameObject blackPanel; // ブラックアウト用パネル（Inspectorで割り当て）
    [SerializeField] private GameObject whitePanel; // ホワイトアウト用パネル（Inspectorで割り当て）
    private bool didFinalizeWinnerUI = false;        // 勝者演出を1回だけ実行するためのフラグ

    AudioSource audioSource;
    CinemachineImpulseSource impulseSource;
    //ImageNo imageCounter;
    Image blackImage;
    Image whiteImage;

    [SerializeField] AudioClip menuBgm;
    [SerializeField] AudioClip start;
    [SerializeField] List<AudioClip> inGameBgms;
    [SerializeField] AudioClip finishCall;
    [SerializeField] AudioClip suddenDeathBgm;
    [SerializeField] AudioClip resultBgm;


    [SerializeField] public List<GameObject> memberList;
    [SerializeField] public List<GameObject> playerList;
    [SerializeField] public List<GameObject> cpuList;
    bool[] isDead = new bool[4];

    [SerializeField] private Vector3 respawnPosition = new Vector3(0, 0.5f, 0);
    [SerializeField] private float respawnTime = 3.0f;
    private Queue<(GameObject, float)> respawnQueue = new Queue<(GameObject, float)>();
    [SerializeField] private float gameTimer = 90.0f;

    public struct CabinetNeonParameter
    {
        public CabinetNeonState state;
        public Material neonMat;
        public bool isNeonAlterColor;
        public Color neonColor;
        public float neonFlushTimer;
        public float neonFlushIntervalTimer;
    }

    public enum CabinetNeonState
    {
        NeonState_None = -1,
        NeonState_Death,//死んだ
        NeonState_Defeted,//ほかプレイヤーを倒した
        NeonState_SWBreak//スペシャル壁を壊して得点した
    }
    [SerializeField] private List<Material> neonMats = new();
    [SerializeField] private List<Color> neonColors = new();
    private LinkedList<CabinetNeonParameter> cabinetNeons = new LinkedList<CabinetNeonParameter>();
    [SerializeField] private float neonFlushTime = 1.5f;
    [SerializeField] private float neonFlushInterval = 0.1f;


    [SerializeField] private Camera settingCam;
    [SerializeField] private Camera inGameCam;
    [SerializeField] private Camera resultCam;

    [SerializeField] private Material cabinMat;

    [SerializeField] private GameObject settingUI;
    [SerializeField] private GameObject inGameUI;
    [SerializeField] private GameObject settingManager;
    [SerializeField] private GameObject _3DUI;

    int playerNum = 0;
    float timer = 0;

    bool isFadeIn = false;
    [SerializeField] float fadeOutTime = 0.6f;
    [SerializeField] float fadeOutKeepTime = 0.3f;

    [SerializeField] float goBackTitleFadeTime = 1.5f;

    bool isFadeOut = false;
    [SerializeField] float fadeInTime = 0.3f;

    bool isReady = false;
    [SerializeField] float readyTime = 2;

    bool isStart = false;
    [SerializeField] float startTime = 0.5f;

    bool isStartCall = false;
    [SerializeField, Header("GOが出てから開始するまでの時間")] float startCallTime = 1.25f;

    bool isStartCallEnd = false;
    [SerializeField] float startCallEndTime = 2.0f;

    bool isFinish = false;
    [SerializeField] float finishTime = 2;

    bool isSuddenDeath = false;
    [Header("サドンデス")]
    [SerializeField] float initialSuddenDeathUIScale = 0.8f;
    [SerializeField] float targetSuddenDeathUIScale = 0.24f;
    [SerializeField] float suddenDeathUIScaleTime = 1.0f;
    [SerializeField] float suddenDeathUIDisplayTime = 1.0f;
    [SerializeField] float suddenDeathUIFadeOutTime = 0.5f;


    bool isWinner = false;
    [SerializeField] float winnerTime = 1;
    [SerializeField] float resultBoardTime = 6;

    [SerializeField] GetCenterPoint cameraControlPoint;
    [SerializeField, Header("オーディオビジュアライザー群")] List<SpectrumLineControl> specLineCtls;
    [SerializeField] List<SpectrumObjectControl> specObjCtls;

    bool isGameStart = false;
    bool isInitFadeOutEnd = false;
    bool isUno = false;
    Vector3 srcScale;
    Vector3 destScale;
    AudioClip bgmIndex;
    int allleftCount = 0;
    int leftCount = 0;
    int prevLeftCount = 0;
    int winnerIndex = 0;

    public enum eGameState
    {
        GameState_None,
        GameState_BlackOut, GameState_BlackIn,
        GameState_RoundStart,
        GameState_Rounding,
        GameState_Finish,
        GameState_SuddenDeathStart, GameState_SuddenDeathRounding,
        GameState_WhiteOut, GameState_WhiteIn,
        GameState_Result,
        GameState_GoBackTitle,
        GameState_GotoStuffRoll,
    }

    eGameState state = eGameState.GameState_None;

    private List<int> maxScoresIndices = new List<int>();
    private Vector3[] initialPlayerPositions = new Vector3[4];

    private void Awake()
    {
        Application.targetFrameRate = 60;//フレームレート６０で固定

        if (instance == null)
        {
            instance = this;
            //DontDestroyOnLoad(gameObject); // シーン間でマネージャーを保持する場合
        }
        else
        {
            Destroy(gameObject);
        }
    }

    public void SetGameTime(int gameTime)
    {
        gameTimer = gameTime;
    }

    public bool IsGameStarted()
    {
        if (state == eGameState.GameState_Rounding || state == eGameState.GameState_Finish || state == eGameState.GameState_SuddenDeathStart || state == eGameState.GameState_SuddenDeathRounding || state == eGameState.GameState_Result)
        {
            return true;
        }
        return false;
    }

    public bool CanPlayerMove()
    {
        if (state == eGameState.GameState_Rounding || state == eGameState.GameState_SuddenDeathRounding)
        {
            return true;
        }
        return false;
    }

    public float GetCurrentElapsedTime()
    {
        if (!IsGameStarted())
        {
            return 0;
        }
        if (state == eGameState.GameState_Finish)
        {
            return gameTimer;
        }
        return timer;
    }

    public float GetRemainGameTime()
    {
        if (!IsGameStarted())
        {
            return gameTimer;
        }
        if (state == eGameState.GameState_Finish)
        {
            return 0;
        }
        return gameTimer - timer;
    }

    public void SetPlayerNum(int num)
    {
        playerNum = num;
        int count = 0;
        for (int i = 0; i < 4; i++)
        {
            playerList[i].SetActive(false);
            cpuList[i].SetActive(false);

            if (count < num)
            {
                memberList[i] = playerList[i];
                memberList[i].SetActive(true);

            }
            else
            {
                memberList[i] = cpuList[i];
                memberList[i].SetActive(true);

            }
            count++;
        }

        cameraControlPoint.SetTrackPlayerList(memberList);

    }

    public void SetMemberCostume(int[] data)
    {
        memberCostumeIndices = data;
    }

    public int GetPlayerNum()
    {
        return playerNum;
    }

    public List<Material> GetPlayerMaterial()
    {
        return playerMats;
    }

    void Start()
    {
        for (int i = 0; i < memberList.Count; i++)
        {
            if (memberList[i].gameObject.activeInHierarchy)
                allleftCount++;

            isDead[i] = false;
        }
        settingUI.SetActive(true);
        inGameUI.SetActive(false);
        _3DUI.SetActive(true);
        resultTimeline.SetActive(false);
        audioSource = GetComponent<AudioSource>();
        impulseSource = GetComponent<CinemachineImpulseSource>();
        readyText.enabled = false;
        goText.enabled = false;
        suddenDeathText.enabled = false;
        finish.SetActive(false);
        winnerText.SetActive(false);
        drawText.enabled = false;
        timerText.enabled = false;

        // 追加：結果パネル初期化
        if (resultPanel != null) resultPanel.SetActive(false);
        if (blackPanel != null) blackImage = blackPanel.GetComponent<Image>();
        if (whitePanel != null) whiteImage = whitePanel.GetComponent<Image>();


        blackImage.color = new Color(0, 0, 0, 1);
        whiteImage.color = new Color(0, 0, 0, 0);

        settingCam.enabled = true;
        inGameCam.enabled = false;
        resultCam.enabled = false;

        audioSource.PlayOneShot(menuBgm);
        //Pauser.Pause();

        // --- スコア表示 初期化＆自動ひも付け ---
        InitScoreCache();
        if (autoBindScoreFromPlayerGUI) TryAutoBindScoreDisplaysFromPlayerGUI();
        RefreshScoreUI(force: true);

        for (int i = 0; i < neonMats.Count; i++)
        {
            neonMats[i].SetColor("_EmissionColor", neonColors[i]);
        }

        foreach (var slc in specLineCtls)
        {
            slc.enabled = false;
        }

        foreach (var soc in specObjCtls)
        {
            soc.SetInitColor();
            soc.enabled = false;
        }


    }
    
    void Update()
    {
        GameStateRun();

        // スコアを毎フレーム差分更新（GetScore が変われば UI も即反映）
        RefreshScoreUI();
    }


    public void ResetPlayer()
    {
        for (int i = 0; i < memberList.Count; i++)
        {
            if (memberList[i].gameObject.activeInHierarchy)
                allleftCount++;

            isDead[i] = false;
        }
        for (int i = 0; i < playerList.Count; i++) playerList[i].SetActive(false);
        for (int i = 0; i < cpuList.Count; i++) cpuList[i].SetActive(false);
    }
    public void GameStateRun()
    {
        if (state == eGameState.GameState_None)
        {
            if (isInitFadeOutEnd == true && isGameStart == false)
            {
                if (SettingManager.instance.GetSettingState() == eSettingState.SettingState_End)
                {

                    blackImage.enabled = true;

                    state = eGameState.GameState_BlackOut;
                }
                else if (SettingManager.instance.GetSettingState() == eSettingState.SettingState_Quit)
                {
                    blackImage.enabled = true;
                    state = eGameState.GameState_GoBackTitle;
                }
                else if(SettingManager.instance.GetSettingState() == eSettingState.SettingState_StuffRoll)
                {
                    blackImage.enabled = true;
                    state = eGameState.GameState_GotoStuffRoll;
                }
            }
            else if (isInitFadeOutEnd == false)
            {
                timer += Time.deltaTime;
                blackImage.color = new Color(0, 0, 0, Mathf.Lerp(1, 0, timer / fadeOutTime));
                if (timer > fadeOutTime)
                {
                    blackImage.enabled = false;
                    timer = 0.0f;
                    isInitFadeOutEnd = true;

                }
            }
        }

        if(state == eGameState.GameState_GotoStuffRoll)
        {
            timer += Time.deltaTime;
            blackImage.color = new Color(0, 0, 0, Mathf.Min(timer, goBackTitleFadeTime) / goBackTitleFadeTime);
            if (timer > goBackTitleFadeTime)
            {

                blackImage.color = new Color(0, 0, 0, 1);
                timer = 0.0f;
                state = eGameState.GameState_None;
                isInitFadeOutEnd = false;
                // スタッフロールへ遷移する処理
                SceneManager.LoadScene("StuffRoll");//シーンをロードする
                return;
            }
        }
        else if (state == eGameState.GameState_GoBackTitle)
        {
            timer += Time.deltaTime;
            blackImage.color = new Color(0, 0, 0, Mathf.Min(timer, goBackTitleFadeTime) / goBackTitleFadeTime);
            if (timer > goBackTitleFadeTime)
            {

                blackImage.color = new Color(0, 0, 0, 1);
                timer = 0.0f;
                state = eGameState.GameState_None;
                isInitFadeOutEnd = false;
                // タイトルへ戻る処理
                SceneManager.LoadScene("Start");//シーンをロードする
                return;
            }
        }
        else if (state == eGameState.GameState_BlackOut)
        {
            timer += Time.deltaTime;
            blackImage.color = new Color(0, 0, 0, Mathf.Lerp(0, 1, Mathf.Min(timer, fadeOutTime) / fadeOutTime));
            if (timer > fadeOutTime)
            {
                blackImage.color = new Color(0, 0, 0, 1);
                settingCam.enabled = false;
                inGameCam.enabled = true;
            }
            if (timer > fadeOutTime + fadeOutKeepTime)
            {

                timer = 0.0f;
                state = eGameState.GameState_BlackIn;
                settingUI.SetActive(false);
                inGameUI.SetActive(true);
                playerScoreBoard.SetActive(false);
                timerObj.SetActive(false);
                _3DUI.SetActive(false);
                settingManager.SetActive(false);

            }
        }
        else if (state == eGameState.GameState_BlackIn)
        {
            timer += Time.deltaTime;
            blackImage.color = new Color(0, 0, 0, Mathf.Lerp(1, 0, timer / fadeInTime));
            if (timer > fadeInTime)
            {
                blackImage.color = new Color(0, 0, 0, 0);
                blackImage.enabled = false;
                timer = 0.0f;

                readyText.enabled = true;
                VideoPlayer readyVideoPlayer = readyText.GetComponent<VideoPlayer>();

                readyVideoPlayer.Play();
                isGameStart = true;
                isReady = true;
                for (int i = 0; i < memberList.Count; i++)
                {
                    initialPlayerPositions[i] = memberList[i].transform.position;
                }
                state = eGameState.GameState_RoundStart;
            }
        }
        else if (state == eGameState.GameState_RoundStart)
        {
            timer += Time.deltaTime;
            if (isReady && timer > readyTime)
            {
                timer = 0.0f;
                isReady = false;
                isStart = true;
                OECULogging.Log($"Game Start. Players: {GetPlayerNum()}", "GAME_INFO");
            }
            if (isStart)
            {
                audioSource.volume = Mathf.Min(startTime - timer, 0.1f);

                if (timer > startTime)
                {
                    audioSource.Stop();
                    timer = 0.0f;
                    isStart = false;
                    isStartCall = true;
                    audioSource.volume = 0;
                    goText.enabled = true;
                    VideoPlayer goVideoPlayer = goText.GetComponent<VideoPlayer>();
                    goVideoPlayer.Play();
                }
            }
            if (isStartCall)
            {
                if (timer > startCallTime)
                {
                    audioSource.Stop();
                    timer = 0.0f;
                    isStartCall = false;
                    isStartCallEnd = true;
                    isSuddenDeath = false;
                    audioSource.volume = 0.5f;

                    audioSource.PlayOneShot(start);
                    bgmIndex = inGameBgms[Random.Range(0, inGameBgms.Count)];
                    audioSource.PlayOneShot(bgmIndex);
                    leftCount = allleftCount;
                    playerScoreBoard.SetActive(true);
                    timerObj.SetActive(true);
                    timerText.enabled = true;
                    timerText.text = ((int)gameTimer).ToString();
                    /*
                    imageCounter = timerText.GetComponent<ImageNo>();
                    if (imageCounter == null)
                    {
                        imageCounter = timerText.GetComponentInChildren<ImageNo>(true);
                    }
                    if (imageCounter != null)
                    {
                        imageCounter.SetNo((int)gameTimer);
                    }
                     */
                    foreach (var slc in specLineCtls)
                    {
                        slc.enabled = true;
                    }

                    foreach (var soc in specObjCtls)
                    {
                        soc.enabled = true;
                    }
                    state = eGameState.GameState_Rounding;
                }
            }
        }
        else if (state == eGameState.GameState_Rounding)
        {
            if (!audioSource.isPlaying) audioSource.PlayOneShot(bgmIndex);
            if (isStartCallEnd)
            {
                timer += Time.deltaTime;
                if (timer > startCallEndTime)
                {
                    timer = 0.0f;
                    isStartCallEnd = false;
                }
                return;
            }
            //タイマー制御部
            timer += Time.deltaTime;

            if (timerText.enabled == true)
            {
                leftCount = (int)(gameTimer - timer);
                //imageCounter.SetNo(leftCount);
                timerText.text = leftCount.ToString();

                if (3 < leftCount && leftCount <= 10)
                {
                    timerText.color = new Color(1, 0.5f, 0, 1);
                    if (prevLeftCount == leftCount)
                    {
                        prevLeftCount = leftCount;
                    }
                }
                else if (leftCount <= 3)
                {
                    timerText.color = Color.red;
                }
            }

            //筐体制御部
            if (cabinetNeons != null && cabinetNeons.Count != 0)
            {
                var node = cabinetNeons.First;
                while (node != null)
                {
                    var next = node.Next;
                    var neon = node.Value; // structをコピー

                    neon.neonFlushTimer += Time.deltaTime;
                    if (neon.neonFlushTimer > neonFlushTime)
                    {
                        neon.neonMat.SetColor("_EmissionColor", neon.neonColor);
                        cabinetNeons.Remove(node);
                    }
                    else
                    {
                        neon.neonFlushIntervalTimer += Time.deltaTime;
                        if (neon.neonFlushIntervalTimer > neonFlushInterval)
                        {
                            if (neon.state == CabinetNeonState.NeonState_Death)
                            {
                                if (neon.isNeonAlterColor)
                                {

                                    neon.neonMat.SetColor("_EmissionColor", Color.black);
                                }
                                else
                                {
                                    neon.neonMat.SetColor("_EmissionColor", neon.neonColor);
                                }
                            }

                            neon.isNeonAlterColor = neon.isNeonAlterColor ? false : true;
                            neon.neonFlushIntervalTimer = 0.0f;
                        }

                        node.Value = neon;     // 書き戻す

                    }

                    node = next;
                }
            }

            //リスポ－ン制御部
            while (respawnQueue.Count > 0 && respawnQueue.Peek().Item2 <= timer)
            {
                (GameObject player, float respawnTime) = respawnQueue.Dequeue();
                player.transform.position = respawnPosition;
                PlayerEntity script = player.GetComponent<PlayerEntity>();
                script.enabled = true;
                script.Respawn();
                for (int i =0; i< memberList.Count; i++)
                {
                    if (memberList[i] == player)
                    {
                        isDead[i] = false;
                        break;
                    }
                }
            }

            //勝敗判定部
            if (timer >= gameTimer - 1)
            {

                timer = 0.0f;

                audioSource.Stop();

                audioSource.PlayOneShot(finishCall);
                state = eGameState.GameState_Finish;
                finish.SetActive(true);
                playerScoreBoard.SetActive(false);
                timerObj.SetActive(false);

                for (int i = 0; i < neonMats.Count; i++)
                {
                    neonMats[i].SetColor("_EmissionColor", neonColors[i]);
                }
            }

        }
        else if (state == eGameState.GameState_Finish)
        {
            timer += Time.deltaTime;

            if (timer > finishTime)
            {
                if (!isSuddenDeath) {
                    int maxScore = 0;
                    for (int i = 0; i < memberList.Count; i++)
                    {
                        PlayerEntity script = memberList[i].GetComponent<PlayerEntity>();
                        if (script.GetScore() > maxScore)
                        {
                            maxScore = script.GetScore();
                            winnerIndex = i;
                        }
                    }
                    int cnt = 0;
                    for (int i = 0; i < memberList.Count; i++)
                    {
                        PlayerEntity script = memberList[i].GetComponent<PlayerEntity>();
                        if (script.GetScore() == maxScore)
                        {
                            cnt++;
                        }
                    }

                    isUno = (cnt == 1);
                    resultModelRenderer.material = playerMats[winnerIndex];
                    resultCostumeSetter.SetCostume(memberCostumeIndices[winnerIndex]);

                    if (isUno)
                    {

                        timer = 0.0f;
                        state = eGameState.GameState_WhiteOut;
                    }
                    else
                    {
                        // timer = 0.0f;
                        // drawText.enabled = true;
                        // finish.SetActive(false);
                        // state = eGameState.GameState_Result;
                        maxScoresIndices.Clear();
                        int tmpMaxScore = 0;
                        for (int i = 0; i < memberList.Count; i++)
                        {
                            memberList[i].transform.position = initialPlayerPositions[i];
                            PlayerEntity script = memberList[i].GetComponent<PlayerEntity>();
                            if (script.GetScore() > tmpMaxScore)
                            {
                                tmpMaxScore = script.GetScore();
                            }
                        }

                        for (int i = 0; i < memberList.Count; i++)
                        {
                            PlayerEntity script = memberList[i].GetComponent<PlayerEntity>();
                            if (script.GetScore() == tmpMaxScore)
                            {
                                maxScoresIndices.Add(i);
                            } else
                            {
                                memberList[i].SetActive(false);
                            }
                        }

                        timer = 0.0f;
                        finish.SetActive(false);
                        isSuddenDeath = true;
                        foreach (var idx in maxScoresIndices)
                        {
                            memberList[idx].transform.position = initialPlayerPositions[idx];
                            PlayerEntity script = memberList[idx].GetComponent<PlayerEntity>();
                            script.Respawn();
                            script.enabled = true;
                        }

                        WallNotify[] wallNotifies = walls.GetComponentsInChildren<WallNotify>();
                        foreach (var wallNotify in wallNotifies)
                        {
                            wallNotify.ReplaceToNormalWall(true);
                        }

                        suddenDeathText.transform.localScale = new Vector3(initialSuddenDeathUIScale, initialSuddenDeathUIScale, initialSuddenDeathUIScale);
                        suddenDeathText.enabled = true;
                        suddenDeathText.color = new Color(1, 1, 1, 1);

                        audioSource.Stop();
                        audioSource.PlayOneShot(suddenDeathBgm);
                        state = eGameState.GameState_SuddenDeathStart;

                        Debug.Log($"Sudden Death Start. Players: {string.Join(", ", maxScoresIndices)}");
                    }
                } else
                {
                    // Sudden Death からの終了処理
                    timer = 0.0f;
                    state = eGameState.GameState_WhiteOut;
                }
            }

        }
        else if (state == eGameState.GameState_SuddenDeathStart)
        {
            timer += Time.deltaTime;
            if (timer < suddenDeathUIScaleTime)
            {
                float t = timer / suddenDeathUIScaleTime;
                suddenDeathText.transform.localScale = Vector3.Lerp(new Vector3(initialSuddenDeathUIScale, initialSuddenDeathUIScale, initialSuddenDeathUIScale), new Vector3(targetSuddenDeathUIScale, targetSuddenDeathUIScale, targetSuddenDeathUIScale), t);
            } else if (suddenDeathUIScaleTime <= timer && timer < suddenDeathUIScaleTime + suddenDeathUIDisplayTime)
            {
                suddenDeathText.transform.localScale = new Vector3(targetSuddenDeathUIScale, targetSuddenDeathUIScale, targetSuddenDeathUIScale);
            }
            else if (suddenDeathUIScaleTime + suddenDeathUIDisplayTime <= timer && timer < suddenDeathUIScaleTime + suddenDeathUIDisplayTime + suddenDeathUIFadeOutTime)
            {
                float t = (timer - suddenDeathUIScaleTime - suddenDeathUIDisplayTime) / suddenDeathUIFadeOutTime;
                suddenDeathText.color = new Color(1, 1, 1, Mathf.Lerp(1, 0, t));
            }
            else if (timer >= suddenDeathUIScaleTime + suddenDeathUIDisplayTime + suddenDeathUIFadeOutTime && suddenDeathText.enabled)
            {
                suddenDeathText.enabled = false;
                goText.enabled = true;
                VideoPlayer goVideoPlayer = goText.GetComponent<VideoPlayer>();
                goVideoPlayer.Play();
            } else if (timer >= suddenDeathUIScaleTime + suddenDeathUIDisplayTime + suddenDeathUIFadeOutTime + startCallTime)
            {
                timer = 0.0f;
                audioSource.PlayOneShot(start);
                state = eGameState.GameState_SuddenDeathRounding;
            }
        }
        else if (state == eGameState.GameState_SuddenDeathRounding)
        {
            

            //筐体制御部
            if (cabinetNeons != null && cabinetNeons.Count != 0)
            {
                var node = cabinetNeons.First;
                while (node != null)
                {
                    var next = node.Next;
                    var neon = node.Value; // structをコピー

                    neon.neonFlushTimer += Time.deltaTime;
                    if (neon.neonFlushTimer > neonFlushTime)
                    {
                        neon.neonMat.SetColor("_EmissionColor", neon.neonColor);
                        cabinetNeons.Remove(node);
                    }
                    else
                    {
                        neon.neonFlushIntervalTimer += Time.deltaTime;
                        if (neon.neonFlushIntervalTimer > neonFlushInterval)
                        {
                            if (neon.state == CabinetNeonState.NeonState_Death)
                            {
                                if (neon.isNeonAlterColor)
                                {

                                    neon.neonMat.SetColor("_EmissionColor", Color.black);
                                }
                                else
                                {
                                    neon.neonMat.SetColor("_EmissionColor", neon.neonColor);
                                }
                            }

                            neon.isNeonAlterColor = neon.isNeonAlterColor ? false : true;
                            neon.neonFlushIntervalTimer = 0.0f;
                        }
                        node.Value = neon;     // 書き戻す
                    }
                    node = next;
                }
            }

            // 勝敗判定部
            int remainingPlayers = 0;
            int tmpWinnerIndex = maxScoresIndices[0];
            for (int i=0; i<maxScoresIndices.Count; i++)
            {
                if (memberList[maxScoresIndices[i]].activeInHierarchy)
                {
                    PlayerEntity script = memberList[maxScoresIndices[i]].GetComponent<PlayerEntity>();
                    if (script != null && script.GetLifeState() != PlayerEntity.eLifeState.LifeState_Death) {
                        remainingPlayers++;
                        tmpWinnerIndex = maxScoresIndices[i];
                    }
                }
            }
            if (remainingPlayers <= 1)
            {
                timer = 0.0f;
                isUno = true;
                winnerIndex = tmpWinnerIndex;
                resultModelRenderer.material = playerMats[winnerIndex];
                resultCostumeSetter.SetCostume(memberCostumeIndices[winnerIndex]);
                audioSource.Stop();
                audioSource.PlayOneShot(finishCall);
                state = eGameState.GameState_Finish;
                finish.SetActive(true);
                playerScoreBoard.SetActive(false);
                timerObj.SetActive(false);

                for (int i = 0; i < neonMats.Count; i++)
                {
                    neonMats[i].SetColor("_EmissionColor", neonColors[i]);
                }
            }
        }
        else if (state == eGameState.GameState_WhiteOut)
        {
            timer += Time.deltaTime;
            whiteImage.color = new Color(1, 1, 1, Mathf.Lerp(0, 1, timer / fadeOutTime));
            if (timer > fadeOutTime + fadeOutKeepTime)
            {
                whiteImage.color = new Color(1, 1, 1, 1);
                resultTimeline.SetActive(true);
                inGameUI.SetActive(false);
                entities.SetActive(false);
                walls.transform.position += new Vector3(0, -0.4f, 0);
                inGameCam.enabled = false;
                resultCam.enabled = true;
                timer = 0.0f;
                audioSource.PlayOneShot(resultBgm);
                state = eGameState.GameState_WhiteIn;
            }
        }
        else if (state == eGameState.GameState_WhiteIn)
        {
            timer += Time.deltaTime;
            whiteImage.color = new Color(1, 1, 1, Mathf.Lerp(1, 0, Mathf.Min(timer, fadeInTime) / fadeInTime));
            if (timer > fadeInTime)
            {
                whiteImage.color = new Color(1, 1, 1, 0);
                timer = 0.0f;
                inGameUI.SetActive(true);

                finish.SetActive(false);
                state = eGameState.GameState_Result;

            }
        }
        else if (state == eGameState.GameState_Result)
        {
            if (isUno)//勝者が確定したら、
            {

                timer += Time.deltaTime;//リザルト中はタイマーを進める
                if (timer > winnerTime && !didFinalizeWinnerUI)//タイマーがUIを表示する時間になったら
                {
                    winnerText.SetActive(true);//WINNERを出す


                    string scores = "";
                    for (int i = 0; i < memberList.Count; i++)
                    {
                        PlayerEntity script = memberList[i].GetComponent<PlayerEntity>();
                        if (i == memberList.Count - 1)
                        {
                            scores += $"{script.GetScore()}";
                        }
                        else
                        {
                            scores += $"{script.GetScore()}, ";
                        }
                    }

                    OECULogging.Log($"Game End. Winner: {(isUno ? winnerIndex : "DRAW")}, Scores: {scores}", "GAME_INFO");

                    // 勝者UIを中央へ
                    if (winnerIndex >= 0 && winnerIndex < playerGUI.Count && playerGUI[winnerIndex] != null)
                    {
                        playerScoreBoard.SetActive(true);
                        playerGUI[winnerIndex].SetActive(true);
                        var rt = playerGUI[winnerIndex].GetComponent<RectTransform>();
                        if (rt != null)
                            rt.position = new Vector3(Screen.width / 2 + 500, Screen.height / 3 - 200);
                        rt.localScale *= 3;
                    }

                    // 他プレイヤーUIを非アクティブ
                    for (int i = 0; i < playerGUI.Count; i++)
                    {
                        if (i == winnerIndex) continue;
                        if (playerGUI[i] != null) playerGUI[i].SetActive(false);
                    }

                    // タイマー等も消す（必要に応じて）
                    if (timerText != null) timerText.enabled = false;

                    didFinalizeWinnerUI = true; // 1回だけ実行

                }

            }

            if (InputUtil.Instance.IsAnyEastPressed()&& timer > winnerTime+1.0f)//一秒だけ入力は待ってもらう
            {
                blackImage.enabled = true;
                inGameUI.SetActive(false);//ロード前に消す

                blackImage.color = new Color(0, 0, 0, 1);
                timer = 0.0f;
                state = eGameState.GameState_None;
                string currentSceneName = SceneManager.GetActiveScene().name;
                LoadManager.instance.NextScene(currentSceneName);//シーンをロードする



            }
            else if (InputUtil.Instance.IsAnySelectPressed())
            {
                // タイトルへ戻る処理
                SceneManager.LoadScene("Start");//シーンをロードする
            }
        }
    }

    public void generateGeneralImpulse()
    {
        impulseSource.GenerateImpulse();
    }

    public void SetOthersPlayer(GameObject obj, List<GameObject> targetList, List<int> targetIndeices)
    {
        for (int i = 0; i < memberList.Count; i++)
        {
            if (memberList[i] != obj && memberList[i].gameObject.activeInHierarchy)
            {
                targetList.Add(memberList[i]);
                targetIndeices.Add(i);
            }
            else continue;
        }
    }
    public void SetCabinetNeonAnim(PlayerEntity script, CabinetNeonState state)
    {
        CabinetNeonParameter neonParam = new CabinetNeonParameter();
        neonParam.state = state;
        neonParam.isNeonAlterColor = false;
        neonParam.neonMat = script.GetNeonMaterial();
        neonParam.neonColor = neonParam.neonMat.GetColor("_EmissionColor");
        neonParam.neonFlushIntervalTimer = 0.0f;
        neonParam.neonFlushTimer = 0.0f;
        cabinetNeons.AddLast(neonParam);
    }

    public void NotifyIsDead(GameObject obj, bool flag)
    {
        for (int i = 0; i < memberList.Count; i++)
        {
            if (memberList[i] == obj) isDead[i] = flag;
        }
        // leftCount--;
        PlayerEntity script = obj.GetComponent<PlayerEntity>();

        SetCabinetNeonAnim(script, CabinetNeonState.NeonState_Death);

        if (flag == true)
        {
            respawnQueue.Enqueue((obj, timer + respawnTime));
        }
    }

    public bool[] GetIsDead()
    {
        return isDead;
    }

    public bool GetIsDeadbyIndex(int index)
    {
        if (isDead[index] == false) return false;
        else return true;
    }

    public eGameState GetGameState()
    {
        return state;
    }

    // ====== ここから補助メソッド ======

    // スコアキャッシュ配列の初期化
    private void InitScoreCache()
    {
        int n = Mathf.Max(0, memberList != null ? memberList.Count : 0);
        lastScores = new int[n];
        for (int i = 0; i < n; i++) lastScores[i] = -1;
    }

    // playerGUI からテキスト／ImageNo を自動検出してリストへ詰める
    private void TryAutoBindScoreDisplaysFromPlayerGUI()
    {
        if (memberList == null) return;
        int n = memberList.Count;

        // リスト長をプレイヤー数に合わせる（不足は null で埋める）
        EnsureListSize(scoreTexts, n);
        EnsureListSize(scoreNumbers, n);

        for (int i = 0; i < n; i++)
        {
            // 既に Inspector で割り当て済みなら触らない
            if (scoreNumbers[i] == null || scoreTexts[i] == null)
            {
                var root = (playerGUI != null && i < playerGUI.Count) ? playerGUI[i] : null;
                if (root != null)
                {
                    if (scoreNumbers[i] == null)
                    {
                        var num = root.GetComponentInChildren<ImageNo>(true);
                        if (num != null) scoreNumbers[i] = num;
                    }
                    if (scoreTexts[i] == null)
                    {
                        var txt = root.GetComponentInChildren<TMP_Text>(true);
                        if (txt != null) scoreTexts[i] = txt;
                    }
                }
            }
        }
    }

    // リスト長を n に調整（不足分は null を追加）
    private void EnsureListSize<T>(List<T> list, int n) where T : class
    {
        if (list == null) return;
        while (list.Count < n) list.Add(null);
        if (list.Count > n) list.RemoveRange(n, list.Count - n);
    }

    // スコアの差分更新（テキスト／ImageNo どちらでも対応、null 安全）
    private void RefreshScoreUI(bool force = false)
    {
        if (memberList == null || memberList.Count == 0) return;

        // キャッシュ長がズレたら作り直し
        if (lastScores == null || lastScores.Length != memberList.Count) InitScoreCache();

        int count = memberList.Count;
        EnsureListSize(scoreTexts, count);
        EnsureListSize(scoreNumbers, count);

        for (int i = 0; i < count; i++)
        {
            var pe = memberList[i] != null ? memberList[i].GetComponent<PlayerEntity>() : null;
            if (pe == null) continue;

            int s = pe.GetScore();
            if (!force && s == lastScores[i]) continue;

            // ImageNo 優先（スプライト数字）
            if (i < scoreNumbers.Count && scoreNumbers[i] != null)
            {
                scoreNumbers[i].SetNo(s);
            }

            // テキストも残っていれば同期更新（旧 UI 併用時のため）
            if (i < scoreTexts.Count && scoreTexts[i] != null)
            {
                scoreTexts[i].text = s.ToString();
            }

            lastScores[i] = s;
        }
    }
}
