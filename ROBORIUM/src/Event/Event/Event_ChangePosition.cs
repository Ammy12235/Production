using UnityEngine;

public class Event_ChangePosition : IEvent
{
    [SerializeField] private GameObject target;
    [SerializeField] private Transform startTransform;
    [SerializeField] private Transform destTransform;

    [SerializeField] private float transitionTime = 0.0f;
    [SerializeField] private AudioClip clip;
    float timer = 0.0f;

    AudioSource source;

    private void Start()
    {
        target.transform.position = startTransform.position;

        source = GetComponent<AudioSource>();
        

    }
    public override void Init()
    {
        base.Init();
       if(source!=null)
        {
            source.PlayOneShot(clip);
        }
    }

    public override void Run()
    {
        base.Run();
        timer += Time.deltaTime;

        if (timer < transitionTime)
        {
            // 0.0 から 1.0 までの割合を計算
            float t = timer / transitionTime;
            Vector3 newPos = Vector3.Lerp(startTransform.position, destTransform.position, t);
            target.transform.position = newPos;
            GamepadRumbleManager.Instance.PlayRumble(RumbleType.PushSwitch);
        }
        else
        {
            // 最終的なスケールを確実に設定
            target.transform.position = destTransform.position;
            timer = 0.0f;
            Finish();
        }

    }

    public override void Finish()
    {
        base.Finish();
    }
}
