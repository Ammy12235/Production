using UnityEngine;

public class Event_ChangeTime_3 : IEvent
{
    [SerializeField] private TimeState state;
    [SerializeField] private Light dLight;
    [SerializeField] private Material skyBoxMat;
    private Material mat;
    enum TimeState
    {
        State_None,
        State_Morning,
        State_Noon,
        State_Night,

    }
    bool isPushed = false;
    bool isLerpIntensity = false;
    float destIntensity = 0.0f;

    public override void Init()
    {
        base.Init();
        isPushed = true;
        isLerpIntensity = false;

    }

    public override void Run()
    {
        base.Run();

        if (isPushed)
        {
            RenderSettings.skybox = skyBoxMat;

            switch (state)
            {
                case TimeState.State_Morning:
                    iTween.RotateTo(dLight.gameObject, iTween.Hash("x", 30f, "time", 1f));
                    destIntensity = 2.0f;
                    break;
                case TimeState.State_Noon:
                    iTween.RotateTo(dLight.gameObject, iTween.Hash("x", 90f, "time", 1f));
                    destIntensity = 2.5f;
                    break;
                case TimeState.State_Night:
                    iTween.RotateTo(dLight.gameObject, iTween.Hash("x", 150f, "time", 1f));
                    destIntensity = 0.1f;
                    break;
                default:
                    break;


            }

            isLerpIntensity = true;
            isPushed = false;
        }

        if (isLerpIntensity)
        {
            dLight.intensity = iTween.FloatUpdate(dLight.intensity, destIntensity, 5);
            if (dLight.intensity - destIntensity < 0.01f)
            {
                dLight.intensity = destIntensity;
                isLerpIntensity = false;
                Finish();
            }
        }

       

    }

    public override void Finish()
    {
        base.Finish();
    }
}
