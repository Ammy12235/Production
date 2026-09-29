using UnityEngine;

public class RenderTargetClear : MonoBehaviour
{
    [SerializeField] RenderTexture targetRT;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        RenderTexture rt = targetRT;

        RenderTexture prev = RenderTexture.active;
        RenderTexture.active = rt;

        // color = true, depth = true, clearColor = RGBA(0,0,0,0)
        GL.Clear(true, true, Color.clear);

        RenderTexture.active = prev;
    }

    private void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        
    }
}
