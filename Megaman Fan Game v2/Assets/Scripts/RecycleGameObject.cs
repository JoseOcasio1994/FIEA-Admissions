using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public interface IRecylce
{
    void Restart();
    void Shutdown();
}

public class RecycleGameObject : MonoBehaviour {

    private List<IRecylce> recycleComponents;

    void Awake()
    {
        var components = GetComponents<MonoBehaviour>();
        recycleComponents = new List<IRecylce>();

        foreach(var component in components)
        {
            if(component is IRecylce)
            {
                recycleComponents.Add(component as IRecylce);
            }
        }
    }

	public void Restart ()
    {
        gameObject.SetActive(true);

        foreach (var component in recycleComponents)
        {
            component.Restart();
        }
	}
	

	public void Shutdown ()
    {
        gameObject.SetActive(false);

        foreach (var component in recycleComponents)
        {
            component.Shutdown();
        }
    }
}
