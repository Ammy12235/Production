using UnityEngine;
using UnityEngine.Playables;
using UnityEngine.Timeline;

public class Event_Timeline :IEvent
{
    [SerializeField] private TimelineAsset timelineAsset;
    [SerializeField] private Camera enableCamera;
    [SerializeField] private Camera disableCamera;

    PlayableDirector playableDirector;
    private void Awake()
    {
       
        playableDirector=GetComponent<PlayableDirector>();
    }
    public override void Init()
    {
        base.Init();

        enableCamera.enabled = true;
        disableCamera.enabled = false;
        playableDirector.time = 0;
        playableDirector.playableAsset = timelineAsset;
        playableDirector.Play(timelineAsset);
    }

    public override void Run()
    {
        base.Run();


    }

    public override void Finish()
    {
        enableCamera.enabled = false;
        disableCamera.enabled = true;
        base.Finish();
    }
}
