using UnityEngine;

public class EventLineDrawer : MonoBehaviour
{
    [SerializeField] private float lineDisappearTime = 1.0f;//線がどれだけの時間表示されるか
    [SerializeField] private float lineDestroyTime = 1.0f;//線がどれだけの時間たったら消えるか

    float timer = 0.0f;

    float alpha = 1;
    struct Pair
    {
        public Vector3 start;
        public Vector3 end;
    }

    Pair pair;

    public void SetPair(Vector3 start, Vector3 end)
    {
        pair.start = start;
        pair.end = end;
    }
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        timer = 0.0f;
    }

    private void OnDrawGizmos()
    {
        Gizmos.color = Color.magenta;
        Gizmos.DrawLine(pair.start, pair.end);
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.deltaTime;
        if(timer>lineDisappearTime)
        {
            alpha = Mathf.Lerp(0, 1, (timer - lineDisappearTime) / lineDestroyTime);
        }
        if(timer>lineDisappearTime+lineDestroyTime)
        {
            Destroy(gameObject);
        }
    }
}
