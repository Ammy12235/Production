using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UIElements;
/// <summary>
/// ルールからパッドアサインへ
/// </summary>
public class EaseFunc_RtoPA : EaseFunc
{
    [SerializeField] private SkinnedMeshRenderer[] renderers;
    float value = 0;
    float scale = 0;
    [SerializeField] private int extendMaxValue;
    [SerializeField] private int extendSpeed;

    [SerializeField] private GameObject padUI;
    [SerializeField] private float maxScale = 0.003f;
    [SerializeField] private float minScale = 0.003f;
    [SerializeField] private float scalingSpeed;

    [SerializeField] private PadAssignManager padAssignManager;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    public void Start()
    {
        base.Start();
    }
    override public void Proceed()
    {
        base.Proceed();
        value = iTween.FloatUpdate(value, extendMaxValue, extendSpeed);
        renderers[0].SetBlendShapeWeight(0, value);

        scale = iTween.FloatUpdate(padUI.transform.localScale.x, maxScale, scalingSpeed);

        padUI.transform.localScale = new Vector3(scale, scale, scale);
    }

    public override void LateProceed()
    {
        base.LateProceed();
    }

    override public void Reverse()
    {
        base.Reverse();
        value = iTween.FloatUpdate(value, 0, extendSpeed);
        renderers[0].SetBlendShapeWeight(0, value);

        scale = iTween.FloatUpdate(padUI.transform.localScale.x, minScale, scalingSpeed);

        padUI.transform.localScale = new Vector3(scale, scale, scale);
    }
    public override void LateReverse()
    {
        base.LateReverse();
    }

    public override void Entry()
    {
        base.Entry();

        PadAssigner.Instance.enabled = false;
        var playerUI = padAssignManager.GetPlayerUI();
        var cpuUI = padAssignManager.GetCpuUI();

        int count = 0;
        for (int i = 0; i < 4; i++)
        {
            playerUI[i].noAssignUI.SetActive(false);
            playerUI[i].assignUI.SetActive(false);
            playerUI[i].ok.SetActive(false);

            cpuUI[i].SetActive(false);
            if (count < GameManager.instance.GetPlayerNum())
            {
                playerUI[i].noAssignUI.SetActive(true);
            }
            else
            {
                cpuUI[i].SetActive(true);
            }
            count++;
        }
        padAssignManager.OnAssignPlayerEnd();

        padUI.transform.localScale = new Vector3(minScale,minScale, minScale);
        value = 0;
    }

    public override void End()
    {
        base.End();
        PadAssigner.Instance.enabled = true;
        PadAssigner.Instance.ResetAssignments();
        PadAssigner.Instance.ApplyPlayers(GameManager.instance.playerList);
        padAssignManager.OnAssignPlayerStart();
        Debug.Log("assigned pad cleared and please assign gamepad");

    }


}