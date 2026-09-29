using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;


public class RuleSettingManager : MonoBehaviour
{

    [System.Serializable]
    struct textParameter
    {
        public List<string> text;
        public string infoText;
    }

    [SerializeField] private List<textParameter> texts = new();
    [SerializeField] private List<int> rowIndex = new();
    [SerializeField] private List<int> gameTimes;
    [SerializeField] private List<int> playerNum;

    [SerializeField] private List<Text> textBox;
    [SerializeField] private List<Transform> arrowPos;
    [SerializeField] private Transform arrowPivot;
    [SerializeField] private Text infoTextBox;

    int index = 0;

    [SerializeField] private int gameRuleTime = 120;
    [SerializeField] private int gameRuleStock = 3;

    [SerializeField] private AudioSource source;
    [SerializeField] private AudioClip columnSE_down;
    [SerializeField] private AudioClip columnSE_up;
    [SerializeField] private AudioClip rowSE;




    private SettingUI ui;
    bool isTiltedOne = false;
    bool isApply = false;

    //[SerializeField] private List<List<int>> selectedIndex;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        ui = GetComponent<SettingUI>();
       
        //インスペクタなどで設定した文言をテキストボックスに移し替える
        for (int i = 0; i < texts.Count; i++)
        {
            textBox[i].text = texts[i].text[rowIndex[i]];
            infoTextBox.text = texts[i].infoText;
        }

    }

    // Update is called once per frame
    void Update()
    {

        arrowPivot.position = arrowPos[index].position;
        if (ui != null && ui.GetPanelState() == SettingUI.ePanelState.PanelState_Select)
        {

            if (isTiltedOne == false && isApply == false)
            {

                bool valueApply = InputUtil.Instance.IsAnyEastPressed();

                if (valueApply)
                {
                    isApply = true;

                }
                else if (InputUtil.Instance.IsAnyAxisChangedToRight())//右
                {
                    // Debug.Log("Right");
                    if (rowIndex[index] < texts[index].text.Count - 1)
                    {
                        rowIndex[index]++;
                        source.PlayOneShot(rowSE);

                    }


                    isTiltedOne = true;
                }
                else if (InputUtil.Instance.IsAnyAxisChangedToLeft())//左
                {
                    if (rowIndex[index] > 0)
                    {
                        rowIndex[index]--;
                        source.PlayOneShot(rowSE);
                    }
                    isTiltedOne = true;
                }
                else if (InputUtil.Instance.IsAnyAxisChangedToUp())//上（上下の移動でcolumnIndexを切り替える）
                {
                    if (index > 0)
                    {
                        //source.PlayOneShot(columnSE_up);
                        index--;

                    }

                    isTiltedOne = true;
                }
                else if (InputUtil.Instance.IsAnyAxisChangedToDown())//下
                {

                    if (index < texts.Count - 1)
                    {
                        //source.PlayOneShot(columnSE_down);
                        index++;

                    }

                    isTiltedOne = true;
                }
            }
            if (InputUtil.Instance.IsAnyAxisChanged() == false)
            {
                isTiltedOne = false;
            }
            else
            {
                //インスペクタなどで設定した文言をテキストボックスに移し替える
                for (int i = 0; i < texts.Count; i++)
                {
                    textBox[i].text = texts[i].text[rowIndex[i]];
                    infoTextBox.text = texts[i].infoText;
                }
            }

            if (isApply)
            {
                //設定したルールを適応する
                GameManager.instance.ResetPlayer();
                GameManager.instance.SetGameTime(gameTimes[rowIndex[1]]);
                GameManager.instance.SetPlayerNum(playerNum[rowIndex[2]]);


                isApply = false;
            }
        }
    }
}
