using UnityEngine;

public class Event_ChangeScale : IEvent
{
    [SerializeField] private GameObject target;

    [SerializeField] private Vector3 destScale;

    private Vector3 startScale;
    [SerializeField] private float transitionTime = 0.0f;
    float timer = 0.0f;
    public override void Init()
    {
        base.Init();
        startScale = target.transform.localScale;
    }

    public override void Run()
    {
        base.Run();
        timer += Time.deltaTime;

        if (timer < transitionTime)
        {
            // 0.0 から 1.0 までの割合を計算
            float t = timer / transitionTime;
            Vector3 newScale = Vector3.Lerp(startScale, destScale, t);
            target.transform.localScale = newScale;
        }
        else
        {
            // 最終的なスケールを確実に設定
            target.transform.localScale = destScale;
            timer = 0.0f;
            Finish();
        }

    }

    public override void Finish()
    {
        base.Finish();
    }
}
