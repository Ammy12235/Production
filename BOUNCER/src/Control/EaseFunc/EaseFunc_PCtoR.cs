using UnityEngine;
using UnityEngine.UIElements;
using static UnityEngine.Rendering.DebugUI;
public class EaseFunc_PCtoR : EaseFunc
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
    public void Start()
    {

        base.Start();
    }

    override public void Proceed()
    {
        base.Proceed();

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
    }

    public override void End()
    {
        
        base.End();

    }
}

