using System;
using System.Collections;
using UnityEngine;
using UnityEngine.UI;

public class GimmickEventManager : MonoBehaviour
{
    public static GimmickEventManager Instance { get; private set; }

    [SerializeField]
    private GimmickEvent[] gimmickEvents;

    [SerializeField]
    private Image eventUIImage;
    
    [SerializeField, Header("フェードにかかる時間")]
    private float fadeDuration = 0.5f;
    
    [SerializeField, Header("最小表示時間")]
    private float minDisplayDuration = 4f;
    
    private Coroutine fadeCoroutine;

    void Awake()
    {
        if (Instance != null && Instance != this)
        {
            Destroy(this.gameObject);
        }
        else
        {
            Instance = this;
        }
    }

    void Start() {
        if (eventUIImage != null)
        {
            eventUIImage.color = new Color(1f, 1f, 1f, 0f);
        }
    }

    void Update()
    {
        if (GameManager.instance.GetGameState() != GameManager.eGameState.GameState_Rounding)
        {
            return;
        }
        
        GimmickEvent[] availableGimmickEvents = Array.FindAll(gimmickEvents, gimmickEvent => gimmickEvent.CheckExecutionCondition());
        
        if (availableGimmickEvents.Length != 0)
        {
            int randomIndex = UnityEngine.Random.Range(0, availableGimmickEvents.Length);
            ShowEventUI(availableGimmickEvents[randomIndex].GetEventSprite(), Mathf.Max(minDisplayDuration, availableGimmickEvents[randomIndex].GetUIDuration()));
            availableGimmickEvents[randomIndex].ActivateGimmick();
        }
    }
    
    void ShowEventUI(Sprite sprite, float displayDuration)
    {
        if (eventUIImage == null)
        {
            return;
        }
        
        eventUIImage.sprite = sprite;
        eventUIImage.SetNativeSize();
        
        eventUIImage.color = new Color(1f, 1f, 1f, 0f);
        
        if (fadeCoroutine != null)
        {
            StopCoroutine(fadeCoroutine);
        }
        fadeCoroutine = StartCoroutine(FadeEventUI(displayDuration));
    }

    IEnumerator FadeEventUI(float displayDuration) {
        float elapsedTime = 0f;
        while (elapsedTime < fadeDuration) {
            float alpha = Mathf.Lerp(0f, 1f, elapsedTime / fadeDuration);
            eventUIImage.color = new Color(1f, 1f, 1f, alpha);
            elapsedTime += Time.deltaTime;
            yield return null;
        }
        eventUIImage.color = new Color(1f, 1f, 1f, 1f);
        
        yield return new WaitForSeconds(displayDuration);
        
        elapsedTime = 0f;
        while (elapsedTime < fadeDuration) {
            float alpha = Mathf.Lerp(1f, 0f, elapsedTime / fadeDuration);
            eventUIImage.color = new Color(1f, 1f, 1f, alpha);
            elapsedTime += Time.deltaTime;
            yield return null;
        }
        eventUIImage.color = new Color(1f, 1f, 1f, 0f);
        
        fadeCoroutine = null;
    }

    void OnDestroy()
    {
        if (Instance == this)
        {
            Instance = null;
            if (fadeCoroutine != null)
            {
                StopCoroutine(fadeCoroutine);
                if (eventUIImage != null) {
                    eventUIImage.color = new Color(1f, 1f, 1f, 0f);
                }
            }
        }
    }


}