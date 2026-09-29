using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class PlayerCustomManager : MonoBehaviour
{
    [SerializeField] private List<PlayerCustomPanel> panels;

    /*
    [System.Serializable]
    public struct costumeParameter
    {
        public GameObject costumeObj;
        public Transform costumeTransform;
    }

    [SerializeField] private List<costumeParameter> costumes;
     */

    [SerializeField] private List<Texture> patterns;

    [SerializeField] private List<Color> subDeepColors;
    [SerializeField] private List<Color> subColors;

    [SerializeField] private List<PlayerInput> playerInputs;

    [SerializeField] private List<CostumeSetter> playerSetters;
    [SerializeField] private List<CostumeSetter> cpuSetters;

    [SerializeField] private GameObject startCall;
    [SerializeField] private AudioSource source;
    [SerializeField] private AudioClip startCallIn;
    [System.Serializable]
    public struct textParameter
    {
        public List<string> text;
        public string infoText;
    }

    int applyCount = 0;
    enum playerCustomState
    {
        State_None,
        State_Waiting,
        State_StartReady,
    }
    [SerializeField] private List<textParameter> texts = new();

    private int[] costumeIndices=new int[4];

    SettingUI ui;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        ui = GetComponent<SettingUI>();
        for (int i = 0; i < panels.Count; i++)
        {
            panels[i].gameObject.SetActive(true);
            panels[i].gameObject.GetComponent<PlayerCustomPanel>().SetPlayerCustomManager(this);


        }
       
    }

    public void SetPlayerInput(int num)
    {
        for (int i = 0; i < num; i++)
        {
            panels[i].SetPlayerInput(PadAssigner.Instance.GetPlayerInputByIndex(i));
        }
    }

    // Update is called once per frame
    void Update()
    {


        if (ui != null && ui.GetPanelState() == SettingUI.ePanelState.PanelState_Select)
        {
            
            for (int i = 0; i < panels.Count; i++)
            {

                panels[i].UpdatePanel();

            }
            
        }

    }

    public List<textParameter> GetText()
    {
        return texts;
    }

    public int[] GetCostumeIndices()
    {
        return costumeIndices;
    }

    public void AllChange()
    {
        for (int i = 0; i < panels.Count; i++)
        {
            NotifyChange(panels[i]);
        }


    }
    public void CPUCostumeChange()
    {
        int playerNum = GameManager.instance.GetPlayerNum();

        for (int i = playerNum; i < 4; i++)
        {
            int index = Random.Range(0, texts[0].text.Count);
            //コスチューム
            cpuSetters[i].SetCostume(index);
            costumeIndices[i] = index;
            List<Material> playerMaterials = GameManager.instance.GetPlayerMaterial();
            //模様
            playerMaterials[i].SetTexture("_Pattern", patterns[Random.Range(0, texts[1].text.Count)]);

            //サブカラー
            index = Random.Range(0, texts[2].text.Count);
            if (index == 0)//インデックス０の場合
            {
                playerMaterials[i].SetColor("_SubColor", subDeepColors[i]);
            }
            else
                playerMaterials[i].SetColor("_SubColor", subColors[index - 1]);
        }
    }
    //パネル操作によって表示させる内容を変える必要があるならこれを呼び出す
    public void NotifyChange(PlayerCustomPanel panelScript)
    {
        if (panelScript == null) return;


        int index = panelScript.GetPanelIndex();
        List<int> data = panelScript.GetSelectData();
        if (panelScript.GetIsApply())
        {
            applyCount++;
        }


        if (applyCount >= GameManager.instance.GetPlayerNum())
        {
            startCall.SetActive(true);
            source.PlayOneShot(startCallIn);
            ui.SetIsChooseEnabled(true);
            ui.SetIsUndoEnabled(false);
        }
        else
        {
            ui.SetIsChooseEnabled(false);
            ui.SetIsUndoEnabled(true);
            startCall.SetActive(false);
        }

        //コスチューム
        playerSetters[index].SetCostume(data[0]);
        costumeIndices[index] = data[0];
        List<Material> playerMaterials = GameManager.instance.GetPlayerMaterial();
        //模様
        playerMaterials[index].SetTexture("_Pattern", patterns[data[1]]);

        //サブカラー
        if (data[2] == 0)//インデックス０の場合
        {
            playerMaterials[index].SetColor("_SubColor", subDeepColors[index]);
        }
        else
            playerMaterials[index].SetColor("_SubColor", subColors[data[2] - 1]);
    }
}
