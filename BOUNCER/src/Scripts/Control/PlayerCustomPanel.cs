using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.UI;

public class PlayerCustomPanel : MonoBehaviour
{

    [SerializeField] private int panelIndex;
    [SerializeField] private RenderTexture playerRT;
    [SerializeField] private RawImage playerRTImage;
    [SerializeField] private GameObject OK;


    [SerializeField] private List<int> rowIndex = new();

    [SerializeField] private List<Text> textBox;
    [SerializeField] private List<Transform> arrowPos;
    [SerializeField] private Transform arrowPivot;
    [SerializeField] private Text infoTextBox;

    int index = 0;

    PlayerInput playerInput;

    [SerializeField] private AudioSource source;
    [SerializeField] private AudioClip rowSE;
    [SerializeField] private AudioClip applySE;
    [SerializeField] private AudioClip undoSE;


    PlayerCustomManager playerCustomManager;


    bool isTiltedOne = false;
    bool isApply = false;
    bool prevIsApply = false;

    public List<int> GetSelectData()
    {
        return rowIndex;
    }

    public int GetPanelIndex()
    {
        return panelIndex;
    }
    public bool GetIsApply()
    {
        if (!prevIsApply && isApply) return true;
        else return false;
    }

    public bool GetIsUndo()
    {
        if (prevIsApply && !isApply) return true;
        else return false;
    }

    public void SetPlayerCustomManager(PlayerCustomManager manager)
    {
        if (manager == null) return;
        playerCustomManager = manager;
        var texts = playerCustomManager.GetText();
        for (int i = 0; i < rowIndex.Count; i++)
        {
            rowIndex[i] = 0;
        }
        //インスペクタなどで設定した文言をテキストボックスに移し替える
        for (int i = 0; i < texts.Count; i++)
        {
            textBox[i].text = texts[i].text[rowIndex[i]];
            infoTextBox.text = texts[i].infoText;
        }
        playerRTImage.texture = playerRT;
    }

    public void SetPlayerInput(PlayerInput input)
    {
        if (input != null)
            playerInput = input;
        else playerInput = null;
    }
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        OK.SetActive(false);

    }

    // Update is called once per frame
    public void UpdatePanel()
    {
        if (playerInput == null) return;
        arrowPivot.position = arrowPos[index].position;
        prevIsApply = isApply;

        Vector2 axisInput = playerInput.actions["Move"].ReadValue<Vector2>();
       

        if (isTiltedOne == false && isApply == false)
        {

            bool valueApply = playerInput.actions["Charge"].WasPressedThisFrame();
            if (valueApply)
            {
                isApply = true;
                OK.SetActive(true);
                source.PlayOneShot(applySE);
                playerCustomManager.NotifyChange(this);


            }
            else if (axisInput.x>0.5f)//右
            {
                // Debug.Log("Right");
                if (rowIndex[index] < playerCustomManager.GetText()[index].text.Count - 1)
                {
                    rowIndex[index]++;
                    source.PlayOneShot(rowSE);
                    ChangeText();

                    playerCustomManager.NotifyChange(this);
                }


                isTiltedOne = true;
            }
            else if (axisInput.x < -0.5f)//左
            {
                if (rowIndex[index] > 0)
                {
                    rowIndex[index]--;
                    source.PlayOneShot(rowSE);
                    ChangeText();

                    playerCustomManager.NotifyChange(this);

                }
                isTiltedOne = true;
            }
            else if (axisInput.y > 0.5f)//上（上下の移動でcolumnIndexを切り替える）
            {
                if (index > 0)
                {
                    //source.PlayOneShot(columnSE_up);
                    index--;

                }

                isTiltedOne = true;
            }
            else if (axisInput.y < -0.5f)//下
            {

                if (index < playerCustomManager.GetText().Count - 1)
                {
                    //source.PlayOneShot(columnSE_down);
                    index++;

                }

                isTiltedOne = true;
            }
        }
        if (axisInput.magnitude<=0.4f)
        {
            isTiltedOne = false;
        }


    }

    private void ChangeText()
    {
        var texts = playerCustomManager.GetText();
        //インスペクタなどで設定した文言をテキストボックスに移し替える
        for (int i = 0; i < texts.Count; i++)
        {
            textBox[i].text = texts[i].text[rowIndex[i]];
            infoTextBox.text = texts[i].infoText;
        }
    }
}
