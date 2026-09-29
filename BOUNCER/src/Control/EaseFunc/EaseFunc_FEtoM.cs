using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.Rendering.Universal;
/// <summary>
/// ファーストエントリーからメニューへ
/// </summary>
public class EaseFunc_FEtoM : EaseFunc
{
    [SerializeField] private Volume postProcessVolume;

    private Bloom bloom;
    [SerializeField] private float bloomStartValue;
    [SerializeField] private float bloomDestValue;
    [SerializeField] private float bloomEaseSpeed;

    public void Start()
    {

        base.Start();
        PadAssigner.Instance.enabled = false;
        postProcessVolume.profile.TryGet(out bloom);
        if (bloom == null) Debug.Log("No Bloom");
        else bloom.intensity.value = bloomStartValue;
    }

    override public void Proceed()
    {
        base.Proceed();
        if(bloom!=null)
        bloom.intensity.value = iTween.FloatUpdate(bloom.intensity.value, bloomDestValue, bloomEaseSpeed);
    }

    public override void LateProceed()
    {
        base.LateProceed();
    }

    override public void Reverse()
    {
        base.Reverse();
    }

    public override void LateReverse()
    {
        base.LateReverse();
    }

    public override void Entry()
    {
        base.Entry();
        bloom.intensity.value = bloomStartValue;
    }

    public override void End()
    {
        base.End();
    }
}
